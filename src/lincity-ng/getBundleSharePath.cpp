
#ifdef __APPLE__

 /*
 Function : getBundleSharePath()
 Role : localisation shared datas in a bundle
 Parameter : Name to add at the end of return path
 Return : path to th data of the bundle
 Author : Thierry Maillard - thierry dot maillard500 at orange dot fr
 Date : 25/3/2007
 */
 
 #include <CoreFoundation/CFBundle.h>
 #include <string.h>
 #include <sstream>
 #include <stdexcept>
 
 char *getBundleSharePath(char *packageName)
 {
 	CFBundleRef bundle;
 	CFURLRef bundle_url;
 	CFStringRef sr;
 	static char path[FILENAME_MAX];
 
 	if( !(bundle = CFBundleGetMainBundle()) )
 	{
         	std::stringstream msg;
         	msg << "Can't get application name : CFBundleGetMainBundle()";
         	throw std::runtime_error(msg.str());
 	}
 
 	if( !(bundle_url = CFBundleCopyBundleURL( bundle )) )
 	{
         	std::stringstream msg;
         	msg << "Can't get application name : CFBundleCopyBundleURL()";
         	throw std::runtime_error(msg.str());
 	}
 
 	if( !(sr = CFURLCopyFileSystemPath( bundle_url, kCFURLPOSIXPathStyle )) )
 	{
 		CFRelease( bundle_url );
         	std::stringstream msg;
         	msg << "Can't get application name : CFURLCopyFileSystemPath()";
         	throw std::runtime_error(msg.str());
 	}
 
 	if( !CFStringGetCString( sr, path, FILENAME_MAX, kCFStringEncodingASCII ) )
 	{
 		CFRelease( bundle_url );
 		CFRelease( sr );
         	std::stringstream msg;
         	msg << "Can't get application name : CFStringGetCString()";
         	throw std::runtime_error(msg.str());
 	}
 
 	CFRelease( bundle_url );
 	CFRelease( sr );
 
 	(void)strcat(path, "/Contents/Resources/share/");
 	(void)strcat(path, packageName);
 	return path;
 } /* char *getBundleSharePath() */

#endif
