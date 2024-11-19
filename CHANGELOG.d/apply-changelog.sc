#!/usr/bin/env amm
// This script requires Ammonite, running Scala 2.13 and Java 11+

import $ivy.`info.picocli:picocli:4.7+`

import java.nio.file.Files
import java.nio.file.Path
import java.nio.file.StandardCopyOption
import java.util.Optional
import java.util.concurrent.Callable
import picocli.CommandLine
import picocli.CommandLine.Command
import picocli.CommandLine.{Option => PicoOption}
import picocli.CommandLine.Parameters
import scala.io.Source
import scala.jdk.StreamConverters._
import scala.util.Using

sealed abstract class Subheader(val text: String)
object Subheader {
  final case object Gameplay extends Subheader("Gameplay")
  final case object Interface extends Subheader("User Interface")
  final case object Internal extends Subheader("Internal")
  final case object Documentation
    extends Subheader("Documentation / Translation")

  val list = Seq(
    Subheader.Gameplay,
    Subheader.Interface,
    Subheader.Internal,
    Subheader.Documentation
  )

  def fromString(text: String): Subheader = text match {
    case Gameplay.text => Gameplay
    case Interface.text => Interface
    case Internal.text => Internal
    case Documentation.text => Documentation
    case "Documentation" | "Translation" => Documentation
    case _ => throw new IllegalArgumentException(s"unknown subheader: $text")
  }
}
implicit def SubheaderIsOrdered(subheader: Subheader): Ordered[Subheader] = {
  new Ordered[Subheader]() {
    def compare(other: Subheader): Int = {
      Subheader.list.indexOf(subheader).compare(  Subheader.list.indexOf(other))
    }
  }
}

final class Entry(
  val text: Seq[String],
  val version: String,
  val subheader: Subheader
) {}

@Command(name = "apply-changelog")
object ApplyChangelog extends java.util.concurrent.Callable[Int] {
  def apply(): Int = {
    val clDir = Path.of("CHANGELOG.d")
    val changeFiles = Files.list(clDir).toScala(LazyList)
      .filter(_ != clDir.resolve("apply-changelog.sc")) // skip this script
      .filter(_ != clDir.resolve("template.md")) // skip the template
    val entries = changeFiles
      .flatMap(p =>
        Source.fromFile(p.toFile).getLines
        .map(_.stripTrailing)
        // do actual parsing
        .scanLeft[(Seq[String], Option[String], Option[Subheader])](
          (Seq(), None, None)
        ) {(s, l) =>
          if(l.startsWith("## ")) // version header
            (Seq(), Some(l.substring(3)), None)
          else if(l.startsWith("###### ")) // subheader
            (Seq(), s._2, Some(Subheader.fromString(l.substring(7))))
          else if(l.startsWith("- ")) // start entry
            (Seq(l), s._2, s._3)
          else if(l.startsWith("  ") && s._1.nonEmpty) // continue entry
            (s._1 :+ l, s._2, s._3)
          else if(l == "") // blank line
            (Seq(), s._2, s._3)
          else // bad line
            throw new RuntimeException(s"line has bad format: '$l'")
        }
        // drop non-entries
        .filter(_._1.nonEmpty)
        // drop entries that are continued later
        .concat(Seq((Seq(), None, None))) // pad for sliding
        .sliding(2)
        .filter(l => l(0)._1.sizeIs >= l(1)._1.size)
        .map(_(0))
        // assume Unreleased version
        .map(l => (l._1, l._2.getOrElse("Unreleased"), l._3))
        // ensure subheader is specified
        .map(l => (l._1, l._2, l._3.getOrElse{
          throw new RuntimeException("entry does not have a subheader")}))
        // convert to an Entry object
        .map(l => new Entry(l._1, l._2, l._3))
      )

    // only support adding to the 'Unreleased' version for now
    entries.map(_.version).find(_ != "Unreleased").foreach(v =>
      throw new RuntimeException(s"only 'Unreleased' version supported: '$v'"))

    entries.map(_.version).find(_ == "Change or delete me.").foreach{t =>
      throw new RuntimeException(s"encountered placeholder entry: '$t'")
    }

    val groupedEntries = entries.groupBy(_.subheader)
    val targetSubheaders = Subheader.list
      .filter(h => groupedEntries.contains(h))

    Using.resources(
      Files.newBufferedReader(Path.of("CHANGELOG.md")),
      Files.newBufferedWriter(Path.of("CHANGELOG_new.md"))
    ) {(clOld, clNew) =>
      var curVersion: Option[String] = None
      var curHeader: Option[Subheader] = None
      var curLine: String = clOld.readLine()
      var blanks: Int = 0
      def nextLine() = {
        if(curLine == "") {
          blanks += 1
        }
        else if(curLine != null) {
          (0 until blanks).foreach(_ => clNew.newLine())
          blanks = 0
          clNew.write(curLine)
          clNew.newLine()
        }
        curLine = clOld.readLine()
        curLine
      }

      for(targetHeader <- targetSubheaders) {
        // seek to the version
        if(!curVersion.exists(_ == "Unreleased")) {
          while(curLine != null && !curLine.startsWith("## ")) {
            nextLine()
          }
          if(curLine == "## Unreleased") {
            nextLine()
          }
          else {
            clNew.write("## Unreleased")
            clNew.newLine()
            blanks = 2 // in case we're prepending to the beginning of the file
          }
          curVersion = Some("Unreleased")
          curHeader = None
        }

        // seek to the subheader
        while(!curHeader.exists(_ == targetHeader)) {
          while(curLine != null &&
            !curLine.startsWith("## ") &&
            !curLine.startsWith("###### ")
          ) {
            nextLine()
          }

          if(curLine.startsWith("###### ")) {
            curHeader = Some(Subheader.fromString(curLine.substring(7)))
          }
          else if(curLine.startsWith("## ")) {
            curHeader = None
          }

          if(curHeader.exists(_ <= targetHeader)) {
            do {
              nextLine()
            } while(curLine != null && curLine.matches("[- ] .+"))
          }
          else {
            clNew.newLine()
            clNew.write("###### " + targetHeader.text)
            clNew.newLine()
            curHeader = Some(targetHeader)
          }
        }

        groupedEntries(targetHeader).foreach(_.text.foreach{t =>
          clNew.write(t)
          clNew.newLine()
        })
      }

      (0 until blanks).foreach(_ => clNew.newLine())
      clNew.write(curLine)
      clNew.newLine()
      clOld.transferTo(clNew)
    }

    Files.move(
      Path.of("CHANGELOG_new.md"),
      Path.of("CHANGELOG.md"),
      StandardCopyOption.REPLACE_EXISTING
    )

    changeFiles.foreach(Files.delete(_))

    0
  }

  def call = apply
}

@main
def main(args: String*): Unit =
  System.exit(new CommandLine(ApplyChangelog)
    .execute(args: _*))
