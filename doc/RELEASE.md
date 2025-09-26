## Source+GitHub Release
 1. Set a shell variable to the release number for use in following steps.
    - `version=x.x.x`
 2. If this is a minor release, create the release branch.
    - `[ ${version#*.*.} = 0 ] && git branch lincity-ng-${version%.*}`
 3. Ensure all desired feature branches are merged into the release branch.
    - `git log --graph lincity-ng-${version%.*}`
 4. Checkout the release branch and ensure working tree is clean.
    - `git checkout lincity-ng-${version%.*} && git status`
 5. Build and test with a non-release build type (assertions enabled).
    - Ensure the game launches, a game can be loaded, and there are no obvious visual defects.
 6. Apply changelog, re-order entries as needed, ensure consistent formatting and correct grammar, and check that all substantial changes are listed.
    - `./CHANGELOG.d/apply-changelog.sc`
 7. Version bump CMakeLists.txt.
 8. Change the top header of CHANGELOG.md from "Unreleased" to "LinCity-NG x.x.x" and add the date underneath. If there are security fixes, add "[SECURITY]" to the header.
 9. Add a release tag to data/io.github.lincity_ng.lincity-ng.metainfo.xml with the date.
    - `xsltproc --stringparam version $version --stringparam date $(date -uI) -o data/io.github.lincity_ng.lincity-ng.metainfo.xml contrib/add-release.xsl data/io.github.lincity_ng.lincity-ng.metainfo.xml`
10. Stage changes and commit.
    - `git add .`
    - `git commit -m "version x.x.x"`
11. Create a signed tag for the release.
    - `git tag -sm "LinCity-NG x.x.x" lincity-ng-x.x.x`
12. Configure and build Linux and Windows release packages and a source package.
    - `cmake -B build/linux/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr/local -DLINCITYNG_RELOCATABLE=ON`
    - `cmake --build build/linux/ -j12 -t package package_source`
    - `x86_64-w64-mingw32-cmake -B build/win64/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="C:\\Program Files\\lincity-ng" -DLINCITYNG_RELOCATABLE=ON`
    - `cmake --build build/win64/ -j12 -t package`
13. Test Linux and Windows release builds.
    - `./build/linux/bin/lincity-ng --config run/lincity-ng.config`
    - `x86_64-w64-mingw32-wine ./build/win64/bin/lincity-ng.exe --config run/lincity-ng.config`
    - Ensure the game launches, a game can be loaded, and there are no obvious visual defects.
14. If releasing all changes currently on master, update the master branch.
    - `git branch --points-at HEAD^ | grep -xFqe '  master' && git branch -f master`
15. Start a draft GitHub release.
    - title "LinCity-NG x.x.x"
    - Paste the most recent changelog notes into the body (don't include the version header).
16. Attach packages to the GitHub release
    - build/linux/lincity-ng-x.x.x-Linux.tar.xz
    - build/linux/lincity-ng-x.x.x-Linux.tar.xz.sha256
    - build/linux/lincity-ng-x.x.x-Source.tar.xz
    - build/linux/lincity-ng-x.x.x-Source.tar.xz.sha256
    - build/win64/lincity-ng-x.x.x-win64.zip
    - build/win64/lincity-ng-x.x.x-win64.zip.sha256
    - build/win64/lincity-ng-x.x.x-win64.exe
    - build/win64/lincity-ng-x.x.x-win64.exe.sha256
17. Double-check that all previous steps are complete.
18. Push to GitHub.
    - `git push -u upstream master lincity-ng-$version lincity-ng-${version%.*}`
19. Reload the GitHub draft release and select the tag "lincity-ng-x.x.x".
20. Publish the GitHub release.


## Flathub Release
1. Wait for @flathubbot to create a pull request on https://github.com/flathub/io.github.lincity_ng.lincity-ng. If that's not viable for some reason, e.g. the checking process is down or it is an urgent hotfix, send a pull request updating the package URL and SHA256 checksum for LinCity-NG source in `io.github.lincity_ng.lincity-ng.yaml` manually.
2. Wait for the test build to complete; @flathubbot will leave comments on the pull request as it progresses.
3. Run the command suggested by the bot to install the built flatpak for testing.
4. Test the flatpak installation of the game: ensure the game launches, a game can be loaded, and there are no obvious visual defects.
5. Merge the pull request.
