#ifndef HTTP_KERNEL_H
#define HTTP_KERNEL_H

#include <map>
#include <deque>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>


static const struct MimeKeyMap
{
	const char *ext;
	const char *mime;
}
MimeTable[] = 
{
	{ "3dm", "x-world/x-3dmf" },
	{ "3dmf", "x-world/x-3dmf" },
	{ "a", "application/octet-stream" },
	{ "aab", "application/x-authorware-bin" },
	{ "aam", "application/x-authorware-map" },
	{ "aas", "application/x-authorware-seg" },
	{ "abc", "text/vnd.abc" },
	{ "acgi", "text/html" },
	{ "afl", "video/animaflex" },
	{ "ai", "application/postscript" },
	{ "aif", "audio/aiff" },
	{ "aifc", "audio/aiff" },
	{ "aiff", "audio/aiff" },
	{ "aim", "application/x-aim" },
	{ "aip", "text/x-audiosoft-intra" },
	{ "ani", "application/x-navi-animation" },
	{ "aos", "application/x-nokia-9000-communicator-add-on-software" },
	{ "aps", "application/mime" },
	{ "arc", "application/octet-stream" },
	{ "arj", "application/arj" },
	{ "art", "image/x-jg" },
	{ "asf", "video/x-ms-asf" },
	{ "asm", "text/x-asm" },
	{ "asp", "text/asp" },
	{ "asx", "video/x-ms-asf" },
	{ "au", "audio/x-au" },
	{ "avi", "video/avi" },
	{ "avs", "video/avs-video" },
	{ "bcpio", "application/x-bcpio" },
	{ "bin", "application/octet-stream" },
	{ "bm", "image/bmp" },
	{ "bmp", "image/bmp" },
	{ "boo", "application/book" },
	{ "book", "application/book" },
	{ "boz", "application/x-bzip2" },
	{ "bsh", "application/x-bsh" },
	{ "bz", "application/x-bzip" },
	{ "bz2", "application/x-bzip2" },
	{ "c", "text/plain" },
	{ "c++", "text/plain" },
	{ "cat", "application/vnd.ms-pki.seccat" },
	{ "cc", "text/plain" },
	{ "ccad", "application/clariscad" },
	{ "cco", "application/x-cocoa" },
	{ "cdf", "application/cdf" },
	{ "cer", "application/pkix-cert" },
	{ "cer", "application/x-x509-ca-cert" },
	{ "cha", "application/x-chat" },
	{ "chat", "application/x-chat" },
	{ "class", "application/java" },
	{ "com", "text/plain" },
	{ "conf", "text/plain" },
	{ "cpio", "application/x-cpio" },
	{ "cpp", "text/x-c" },
	{ "cpt", "application/x-cpt" },
	{ "crl", "application/pkcs-crl" },
	{ "crt", "application/pkix-cert" },
	{ "csh", "text/x-script.csh" },
	{ "css", "text/css" },
	{ "cxx", "text/plain" },
	{ "dcr", "application/x-director" },
	{ "deepv", "application/x-deepv" },
	{ "def", "text/plain" },
	{ "der", "application/x-x509-ca-cert" },
	{ "dif", "video/x-dv" },
	{ "dir", "application/x-director" },
	{ "dl", "video/dl" },
	{ "doc", "application/msword" },
	{ "docx", "application/msword" },
	{ "dot", "application/msword" },
	{ "dp", "application/commonground" },
	{ "drw", "application/drafting" },
	{ "dump", "application/octet-stream" },
	{ "dv", "video/x-dv" },
	{ "dvi", "application/x-dvi" },
	{ "dwf", "model/vnd.dwf" },
	{ "dwg", "image/x-dwg" },
	{ "dxf", "image/x-dwg" },
	{ "el", "text/x-script.elisp" },
	{ "elc", "application/x-elc" },
	{ "env", "application/x-envoy" },
	{ "eps", "application/postscript" },
	{ "es", "application/x-esrehber" },
	{ "etx", "text/x-setext" },
	{ "evy", "application/envoy" },
	{ "exe", "application/octet-stream" },
	{ "f", "text/plain" },
	{ "f77", "text/x-fortran" },
	{ "f90", "text/plain" },
	{ "fdf", "application/vnd.fdf" },
	{ "fif", "image/fif" },
	{ "fli", "video/fli" },
	{ "flo", "image/florian" },
	{ "flx", "text/vnd.fmi.flexstor" },
	{ "fmf", "video/x-atomic3d-feature" },
	{ "for", "text/plain" },
	{ "fpx", "image/vnd.fpx" },
	{ "frl", "application/freeloader" },
	{ "funk", "audio/make" },
	{ "g", "text/plain" },
	{ "g3", "image/g3fax" },
	{ "gif", "image/gif" },
	{ "gl", "video/gl" },
	{ "gsd", "audio/x-gsm" },
	{ "gsm", "audio/x-gsm" },
	{ "gsp", "application/x-gsp" },
	{ "gss", "application/x-gss" },
	{ "gtar", "application/x-gtar" },
	{ "gz", "application/x-gzip" },
	{ "gzip", "application/x-gzip" },
	{ "h", "text/plain" },
	{ "hdf", "application/x-hdf" },
	{ "help", "application/x-helpfile" },
	{ "hgl", "application/vnd.hp-hpgl" },
	{ "hh", "text/plain" },
	{ "hlb", "text/x-script" },
	{ "hlp", "application/x-helpfile" },
	{ "hpg", "application/vnd.hp-hpgl" },
	{ "hpgl", "application/vnd.hp-hpgl" },
	{ "hqx", "application/binhex" },
	{ "hta", "application/hta" },
	{ "htc", "text/x-component" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "htmls", "text/html" },
	{ "htt", "text/webviewhtml" },
	{ "htx", "text/html" },
	{ "ice", "x-conference/x-cooltalk" },
	{ "ico", "image/x-icon" },
	{ "idc", "text/plain" },
	{ "ief", "image/ief" },
	{ "iefs", "image/ief" },
	{ "iges", "application/iges" },
	{ "iges", "model/iges" },
	{ "igs", "model/iges" },
	{ "ima", "application/x-ima" },
	{ "imap", "application/x-httpd-imap" },
	{ "inf", "application/inf" },
	{ "ins", "application/x-internett-signup" },
	{ "ip", "application/x-ip2" },
	{ "isu", "video/x-isvideo" },
	{ "it", "audio/it" },
	{ "iv", "application/x-inventor" },
	{ "ivr", "i-world/i-vrml" },
	{ "ivy", "application/x-livescreen" },
	{ "jam", "audio/x-jam" },
	{ "jav", "text/plain" },
	{ "java", "text/plain" },
	{ "jcm", "application/x-java-commerce" },
	{ "jfif", "image/jpeg" },
	{ "jfif-tbnl", "image/jpeg" },
	{ "jpe", "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "jpg", "image/jpeg" },
	{ "jps", "image/x-jps" },
	{ "js", "application/x-javascript" },
	{ "jut", "image/jutvision" },
	{ "kar", "audio/midi" },
	{ "ksh", "application/x-ksh" },
	{ "la", "audio/nspaudio" },
	{ "lam", "audio/x-liveaudio" },
	{ "latex", "application/x-latex" },
	{ "lha", "application/lha" },
	{ "lhx", "application/octet-stream" },
	{ "list", "text/plain" },
	{ "lma", "audio/nspaudio" },
	{ "log", "text/plain" },
	{ "lsp", "application/x-lisp" },
	{ "lst", "text/plain" },
	{ "lsx", "text/x-la-asf" },
	{ "lzh", "application/octet-stream" },
	{ "lzx", "application/lzx" },
	{ "m", "text/plain" },
	{ "m1v", "video/mpeg" },
	{ "m2a", "audio/mpeg" },
	{ "m2v", "video/mpeg" },
	{ "m3u", "audio/x-mpequrl" },
	{ "man", "application/x-troff-man" },
	{ "map", "application/x-navimap" },
	{ "mar", "text/plain" },
	{ "mbd", "application/mbedlet" },
	{ "mc$", "application/x-magic-cap-package-1.0" },
	{ "mcd", "application/mcad" },
	{ "mcf", "image/vasa" },
	{ "mcf", "text/mcf" },
	{ "mcp", "application/netmc" },
	{ "me", "application/x-troff-me" },
	{ "mht", "message/rfc822" },
	{ "mhtml", "message/rfc822" },
	{ "mid", "audio/midi" },
	{ "midi", "audio/midi" },
	{ "mif", "application/x-mif" },
	{ "mime", "www/mime" },
	{ "mjf", "audio/x-vnd.audioexplosion.mjuicemediafile" },
	{ "mjpg", "video/x-motion-jpeg" },
	{ "mm", "application/base64" },
	{ "mme", "application/base64" },
	{ "mod", "audio/mod" },
	{ "moov", "video/quicktime" },
	{ "mov", "video/quicktime" },
	{ "movie", "video/x-sgi-movie" },
	{ "mp2", "audio/mpeg" },
	{ "mp3", "audio/mpeg3" },
	{ "mpa", "audio/mpeg" },
	{ "mpc", "application/x-project" },
	{ "mpe", "video/mpeg" },
	{ "mpeg", "video/mpeg" },
	{ "mpg", "video/mpeg" },
	{ "mpga", "audio/mpeg" },
	{ "mpp", "application/vnd.ms-project" },
	{ "mpt", "application/x-project" },
	{ "mpv", "application/x-project" },
	{ "mpx", "application/x-project" },
	{ "mrc", "application/marc" },
	{ "ms", "application/x-troff-ms" },
	{ "mv", "video/x-sgi-movie" },
	{ "my", "audio/make" },
	{ "mzz", "application/x-vnd.audioexplosion.mzz" },
	{ "nap", "image/naplps" },
	{ "naplps", "image/naplps" },
	{ "nc", "application/x-netcdf" },
	{ "ncm", "application/vnd.nokia.configuration-message" },
	{ "nif", "image/x-niff" },
	{ "niff", "image/x-niff" },
	{ "nix", "application/x-mix-transfer" },
	{ "nsc", "application/x-conference" },
	{ "nvd", "application/x-navidoc" },
	{ "o", "application/octet-stream" },
	{ "oda", "application/oda" },
	{ "omc", "application/x-omc" },
	{ "omcd", "application/x-omcdatamaker" },
	{ "omcr", "application/x-omcregerator" },
	{ "p", "text/x-pascal" },
	{ "p10", "application/pkcs10" },
	{ "p10", "application/x-pkcs10" },
	{ "p12", "application/pkcs-12" },
	{ "p12", "application/x-pkcs12" },
	{ "p7a", "application/x-pkcs7-signature" },
	{ "p7c", "application/pkcs7-mime" },
	{ "p7c", "application/x-pkcs7-mime" },
	{ "p7m", "application/pkcs7-mime" },
	{ "p7m", "application/x-pkcs7-mime" },
	{ "p7r", "application/x-pkcs7-certreqresp" },
	{ "p7s", "application/pkcs7-signature" },
	{ "part", "application/pro_eng" },
	{ "pas", "text/pascal" },
	{ "pbm", "image/x-portable-bitmap" },
	{ "pcl", "application/x-pcl" },
	{ "pct", "image/x-pict" },
	{ "pcx", "image/x-pcx" },
	{ "pdb", "chemical/x-pdb" },
	{ "pdf", "application/pdf" },
	{ "pfunk", "audio/make" },
	{ "pgm", "image/x-portable-graymap" },
	{ "pic", "image/pict" },
	{ "pict", "image/pict" },
	{ "pkg", "application/x-newton-compatible-pkg" },
	{ "pko", "application/vnd.ms-pki.pko" },
	{ "pl", "text/plain" },
	{ "plx", "application/x-pixclscript" },
	{ "pm", "image/x-xpixmap" },
	{ "pm4", "application/x-pagemaker" },
	{ "pm5", "application/x-pagemaker" },
	{ "png", "image/png" },
	{ "pnm", "image/x-portable-anymap" },
	{ "pot", "application/mspowerpoint" },
	{ "pov", "model/x-pov" },
	{ "ppa", "application/vnd.ms-powerpoint" },
	{ "ppm", "image/x-portable-pixmap" },
	{ "pps", "application/mspowerpoint" },
	{ "ppt", "application/powerpoint" },
	{ "pptx", "application/powerpoint" },
	{ "ppz", "application/mspowerpoint" },
	{ "pre", "application/x-freelance" },
	{ "prt", "application/pro_eng" },
	{ "ps", "application/postscript" },
	{ "psd", "application/octet-stream" },
	{ "pvu", "paleovu/x-pv" },
	{ "pwz", "application/vnd.ms-powerpoint" },
	{ "py", "text/x-script.phyton" },
	{ "pyc", "applicaiton/x-bytecode.python" },
	{ "qcp", "audio/vnd.qcelp" },
	{ "qd3", "x-world/x-3dmf" },
	{ "qd3d", "x-world/x-3dmf" },
	{ "qif", "image/x-quicktime" },
	{ "qt", "video/quicktime" },
	{ "qtc", "video/x-qtc" },
	{ "qti", "image/x-quicktime" },
	{ "qtif", "image/x-quicktime" },
	{ "ra", "audio/x-realaudio" },
	{ "ram", "audio/x-pn-realaudio" },
	{ "ras", "image/cmu-raster" },
	{ "ras", "image/x-cmu-raster" },
	{ "rast", "image/cmu-raster" },
	{ "rexx", "text/x-script.rexx" },
	{ "rf", "image/vnd.rn-realflash" },
	{ "rgb", "image/x-rgb" },
	{ "rm", "audio/x-pn-realaudio" },
	{ "rmi", "audio/mid" },
	{ "rmm", "audio/x-pn-realaudio" },
	{ "rmp", "audio/x-pn-realaudio" },
	{ "rng", "application/ringing-tones" },
	{ "rnx", "application/vnd.rn-realplayer" },
	{ "roff", "application/x-troff" },
	{ "rp", "image/vnd.rn-realpix" },
	{ "rpm", "audio/x-pn-realaudio-plugin" },
	{ "rt", "text/richtext" },
	{ "rt", "text/vnd.rn-realtext" },
	{ "rtf", "text/richtext" },
	{ "rtx", "text/richtext" },
	{ "rv", "video/vnd.rn-realvideo" },
	{ "s", "text/x-asm" },
	{ "s3m", "audio/s3m" },
	{ "saveme", "application/octet-stream" },
	{ "sbk", "application/x-tbook" },
	{ "scm", "video/x-scm" },
	{ "sdml", "text/plain" },
	{ "sdp", "application/sdp" },
	{ "sdp", "application/x-sdp" },
	{ "sdr", "application/sounder" },
	{ "sea", "application/sea" },
	{ "set", "application/set" },
	{ "sgm", "text/sgml" },
	{ "sgml", "text/sgml" },
	{ "sh", "text/x-script.sh" },
	{ "shar", "application/x-bsh" },
	{ "shtml", "text/html" },
	{ "sid", "audio/x-psid" },
	{ "sit", "application/x-sit" },
	{ "skd", "application/x-koan" },
	{ "skm", "application/x-koan" },
	{ "skp", "application/x-koan" },
	{ "skt", "application/x-koan" },
	{ "sl", "application/x-seelogo" },
	{ "smi", "application/smil" },
	{ "smil", "application/smil" },
	{ "snd", "audio/basic" },
	{ "sol", "application/solids" },
	{ "spc", "text/x-speech" },
	{ "spl", "application/futuresplash" },
	{ "spr", "application/x-sprite" },
	{ "sprite", "application/x-sprite" },
	{ "src", "application/x-wais-source" },
	{ "ssi", "text/x-server-parsed-html" },
	{ "ssm", "application/streamingmedia" },
	{ "sst", "application/vnd.ms-pki.certstore" },
	{ "step", "application/step" },
	{ "stl", "application/sla" },
	{ "stp", "application/step" },
	{ "sv4cpio", "application/x-sv4cpio" },
	{ "sv4crc", "application/x-sv4crc" },
	{ "svf", "image/x-dwg" },
	{ "svr", "x-world/x-svr" },
	{ "swf", "application/x-shockwave-flash" },
	{ "t", "application/x-troff" },
	{ "talk", "text/x-speech" },
	{ "tar", "application/x-tar" },
	{ "tbk", "application/toolbook" },
	{ "tcl", "text/x-script.tcl" },
	{ "tcsh", "text/x-script.tcsh" },
	{ "tex", "application/x-tex" },
	{ "texi", "application/x-texinfo" },
	{ "texinfo", "application/x-texinfo" },
	{ "text", "text/plain" },
	{ "tgz", "application/gnutar" },
	{ "tif", "image/tiff" },
	{ "tiff", "image/tiff" },
	{ "tr", "application/x-troff" },
	{ "tsi", "audio/tsp-audio" },
	{ "tsp", "audio/tsplayer" },
	{ "tsv", "text/tab-separated-values" },
	{ "turbot", "image/florian" },
	{ "txt", "text/plain" },
	{ "json", "application/json"},
	{ "uil", "text/x-uil" },
	{ "uni", "text/uri-list" },
	{ "unis", "text/uri-list" },
	{ "unv", "application/i-deas" },
	{ "uri", "text/uri-list" },
	{ "uris", "text/uri-list" },
	{ "ustar", "application/x-ustar" },
	{ "uu", "application/octet-stream" },
	{ "uue", "text/x-uuencode" },
	{ "vcd", "application/x-cdlink" },
	{ "vcs", "text/x-vcalendar" },
	{ "vda", "application/vda" },
	{ "vdo", "video/vdo" },
	{ "vew", "application/groupwise" },
	{ "viv", "video/vivo" },
	{ "vivo", "video/vivo" },
	{ "vmd", "application/vocaltec-media-desc" },
	{ "vmf", "application/vocaltec-media-file" },
	{ "voc", "audio/voc" },
	{ "vos", "video/vosaic" },
	{ "vox", "audio/voxware" },
	{ "vqe", "audio/x-twinvq-plugin" },
	{ "vqf", "audio/x-twinvq" },
	{ "vql", "audio/x-twinvq-plugin" },
	{ "vrml", "model/vrml" },
	{ "vrt", "x-world/x-vrt" },
	{ "vsd", "application/x-visio" },
	{ "vst", "application/x-visio" },
	{ "vsw", "application/x-visio" },
	{ "w60", "application/wordperfect6.0" },
	{ "w61", "application/wordperfect6.1" },
	{ "w6w", "application/msword" },
	{ "wav", "audio/wav" },
	{ "wb1", "application/x-qpro" },
	{ "wbmp", "image/vnd.wap.wbmp" },
	{ "web", "application/vnd.xara" },
	{ "wiz", "application/msword" },
	{ "wk1", "application/x-123" },
	{ "wmf", "windows/metafile" },
	{ "wml", "text/vnd.wap.wml" },
	{ "wmlc", "application/vnd.wap.wmlc" },
	{ "wmls", "text/vnd.wap.wmlscript" },
	{ "wmlsc", "application/vnd.wap.wmlscriptc" },
	{ "word", "application/msword" },
	{ "wp", "application/wordperfect" },
	{ "wp5", "application/wordperfect" },
	{ "wp5", "application/wordperfect6.0" },
	{ "wp6", "application/wordperfect" },
	{ "wpd", "application/wordperfect" },
	{ "wq1", "application/x-lotus" },
	{ "wri", "application/mswrite" },
	{ "wrl", "application/x-world" },
	{ "wrz", "model/vrml" },
	{ "wsc", "text/scriplet" },
	{ "wsrc", "application/x-wais-source" },
	{ "wtk", "application/x-wintalk" },
	{ "xbm", "image/xbm" },
	{ "xdr", "video/x-amt-demorun" },
	{ "xgz", "xgl/drawing" },
	{ "xif", "image/vnd.xiff" },
	{ "xl", "application/excel" },
	{ "xla", "application/excel" },
	{ "xlb", "application/excel" },
	{ "xlc", "application/excel" },
	{ "xld", "application/excel" },
	{ "xlk", "application/excel" },
	{ "xll", "application/excel" },
	{ "xlm", "application/excel" },
	{ "xls", "application/excel" },
	{ "xlsx", "application/excel" },
	{ "xlt", "application/excel" },
	{ "xlw", "application/excel" },
	{ "xm", "audio/xm" },
	{ "xml", "text/xml" },
	{ "xmz", "xgl/movie" },
	{ "xpix", "application/x-vnd.ls-xpix" },
	{ "xpm", "image/xpm" },
	{ "x-png", "image/png" },
	{ "xsr", "video/x-amt-showrun" },
	{ "xwd", "image/x-xwd" },
	{ "xyz", "chemical/x-pdb" },
	{ "z", "application/x-compressed" },
	{ "zip", "application/zip" },
	{ "zoo", "application/octet-stream" },
	{ "zsh", "text/x-script.zsh" },
	{0,0},
};

template <typename T>
class CHttpKernel
{
public:
	//成员函数指针
	typedef void (T::*HTTP_SERVICE_EVENT_FUNCTOR)(struct evhttp_request *req, void * arg);

protected:
	struct tagServerCallBackInfo
	{
		tagServerCallBackInfo()
		{
			pFunc = NULL;
			pArg = NULL;
		}

		T *	pObject;
		HTTP_SERVICE_EVENT_FUNCTOR pFunc;
		void * pArg;
	};

	struct tagClientCallBackInfo
	{
		tagClientCallBackInfo()
		{
			pFunc = NULL;
			pData = NULL;
			pArg = NULL;
			nSize = 0;
			cbReqType = 0;
			memset(url, 0, sizeof(url));
		}

		T *	pObject;
		HTTP_SERVICE_EVENT_FUNCTOR pFunc;
		unsigned char cbReqType;
		char url[MAX_PATH];
		void * pArg;
		void * pData;
		int nSize;
	};

	typedef std::map<enum evhttp_cmd_type, tagServerCallBackInfo>		ServerCallBackInfo;
	typedef std::deque<tagClientCallBackInfo>							ClientCallBackInfo;

	enum _ServerType_
	{
		SERVER_TYPE_NONE,
		SERVER_TYPE_CLIENT,
		SERVER_TYPE_SERVER,
	};

	enum _ClientState_
	{
		CLIENT_STATE_NONE,
		CLIENT_STATE_FREE,
		CLIENT_STATE_BUSY,
	};


public:
	CHttpKernel()
	{
		base = NULL;
		http = NULL;
		mHttpPort = 0;
		m_cbServerType = SERVER_TYPE_NONE;
		m_cbClientStates = CLIENT_STATE_NONE;
		m_nRetry = 3;
		m_nTimeOut = 10;
	}
	
	virtual ~CHttpKernel()
	{
		if (base)
		{
			event_base_free(base);
			base = NULL;
		}
		if (http)
		{
			evhttp_free(http);
			http = NULL;
		}

#if HTTP_KERNEL_ENABLE_SSL
		if (m_pSSLCtx)
		{
			SSL_CTX_free(m_pSSLCtx);
			m_pSSLCtx = NULL;
		}

#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || defined(LIBRESSL_VERSION_NUMBER)
		EVP_cleanup();
		ERR_free_strings();

#if OPENSSL_VERSION_NUMBER < 0x10000000L
		ERR_remove_state(0);
#else
		ERR_remove_thread_state(NULL);
#endif

		CRYPTO_cleanup_all_ex_data();

		sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
#endif /* (OPENSSL_VERSION_NUMBER < 0x10100000L) || defined(LIBRESSL_VERSION_NUMBER) */
#endif

#ifdef WIN32
		WSACleanup();
#endif
	}

	//在主线程中每帧调用
	virtual bool EventDispatch()
	{
		if (base)
		{
			event_base_loop(base, EVLOOP_ONCE | EVLOOP_NONBLOCK);

			if (m_cbServerType == SERVER_TYPE_CLIENT)
			{
				if (m_cbClientStates != CLIENT_STATE_BUSY)
				{
					if (m_ReqClientFunctors.size())
					{
						tagClientCallBackInfo req = m_ReqClientFunctors.front();
						if (ProcessRequest(req.cbReqType, req.url, req.pData, req.nSize))
						{
							m_cbClientStates = CLIENT_STATE_BUSY;
						}
						else
						{
							if (req.pObject && req.pFunc)
								(req.pObject->*req.pFunc)(NULL, NULL);

							if (req.pData)
								free(req.pData);
							m_ReqClientFunctors.pop_front();
							m_cbClientStates = CLIENT_STATE_FREE;
						}
					}
				}
			}

		}

		return true;
	}

	//作为HTTP Server 初始化模块
	virtual bool InitServer(const unsigned short nPort)
	{
		if (m_cbServerType != SERVER_TYPE_NONE)
			return false;

#ifdef WIN32
		WSADATA WSAData;
		WSAStartup(0x101, &WSAData);
#else
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
			return false;
#endif

		m_cbServerType = SERVER_TYPE_SERVER;

		mHttpPort = nPort;
		//struct event_base *base;

		struct evhttp_bound_socket *handle;

		base = event_base_new();
		if (!base)
		{
			std::cout << "create event_base fail" << std::endl;;
			return false;
		}

		http = evhttp_new(base);
		if (!http) {
			std::cout << "create evhttp fail" << std::endl;;
			return false;
		}

		//通用URL回调函数（在未指定URL回调函数时，调用该函数回调）
		evhttp_set_gencb(http, listener_gencb, this);
		//设置http 120秒超时
		evhttp_set_timeout(http, 120);

		//绑定http端口
		handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", nPort);

		if (!handle)
		{
			std::cout << "bind port :" << mHttpPort << " fail" << std::endl;
			return false;
		}
		else
		{
			std::cout << "bind port :" << mHttpPort << " success" << std::endl;
		}
		return true;
	}
	//作为HTTP Client 初始化模块
	virtual bool InitClient()
	{
		if (m_cbServerType != SERVER_TYPE_NONE)
			return false;

#ifdef WIN32
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		int err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			return false;
		}
#endif

		m_cbServerType = SERVER_TYPE_CLIENT;

		base = event_base_new();
		if (!base)
		{
			std::cout << "create event_base fail" << std::endl;;
			return false;
		}


#if HTTP_KERNEL_ENABLE_SSL

#if OPENSSL_VERSION_NUMBER < 0x10100000L
		// Initialize OpenSSL
		SSL_library_init();
		ERR_load_crypto_strings();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();
#endif

		m_pSSLCtx = SSL_CTX_new(SSLv23_client_method());
		if (!m_pSSLCtx) {
			printf("SSL_CTX_new err.");
			return false;
		}

#endif

		return true;
	}

	//Mime 返回
	static const char* MappedMimeType(const char *path)
	{
		if (!path)
			goto not_found;

		const char *last_period, *extension;
		last_period = strrchr(path, '.');
		if (!last_period || strchr(last_period, '/'))
		{
			goto not_found;
		}

		extension = last_period + 1;
		for (const MimeKeyMap* ent = &MimeTable[0]; ent->ext; ++ent)
		{
			if (!evutil_ascii_strcasecmp(ent->ext, extension))
			{
				return ent->mime;
			}
		}
		goto not_found;

not_found:
		return "application/misc";
	}

	//作为服务端的接口
public:
	//设置Get请求回调
	virtual bool SetReqGetCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_GET);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_GET, info));
		return true;
	}
	//设置Post请求回调
	virtual bool SetReqPostCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_POST);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_POST, info));
		return true;
	}
	//设置Head请求回调
	virtual bool SetReqHeadCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_HEAD);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_HEAD, info));
		return true;
	}
	//设置Put请求回调
	virtual bool SetReqPutCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_PUT);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_PUT, info));
		return true;
	}
	//设置Delete请求回调
	virtual bool SetReqDeleteCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_DELETE);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_DELETE, info));
		return true;
	}
	//设置Options请求回调
	virtual bool SetReqOptionsCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_OPTIONS);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_OPTIONS, info));
		return true;
	}
	//设置Trace请求回调
	virtual bool SetReqTraceCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_TRACE);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_TRACE, info));
		return true;
	}
	//设置Connect请求回调
	virtual bool SetReqConnectCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_CONNECT);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_CONNECT, info));
		return true;
	}
	//设置Patch请求回调
	virtual bool SetReqPatchCallBack(HTTP_SERVICE_EVENT_FUNCTOR functorPtr, T * ptr, void * arg = NULL)
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_PATCH);
		if (it != m_ReqServerFunctors.end())
			m_ReqServerFunctors.erase(it);
		tagServerCallBackInfo info;
		info.pFunc = functorPtr;
		info.pObject = ptr;
		info.pArg = arg;
		m_ReqServerFunctors.insert(std::make_pair(EVHTTP_REQ_PATCH, info));
		return true;
	}

	//获取Get请求回调
	virtual tagServerCallBackInfo GetReqGetCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_GET);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Post请求回调
	virtual tagServerCallBackInfo GetReqPostCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_POST);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Head请求回调
	virtual tagServerCallBackInfo GetReqHeadCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_HEAD);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Put请求回调
	virtual tagServerCallBackInfo GetReqPutCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_PUT);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Delete请求回调
	virtual tagServerCallBackInfo GetReqDeleteCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_DELETE);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Options请求回调
	virtual tagServerCallBackInfo GetReqOptionsCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_OPTIONS);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Trace请求回调
	virtual tagServerCallBackInfo GetReqTraceCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_TRACE);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Connect请求回调
	virtual tagServerCallBackInfo GetReqConnectCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_CONNECT);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	//获取Patch请求回调
	virtual tagServerCallBackInfo GetReqPatchCallBackInfo()
	{
		ServerCallBackInfo::iterator it = m_ReqServerFunctors.find(EVHTTP_REQ_PATCH);
		if (it != m_ReqServerFunctors.end())
			return it->second;
		return tagServerCallBackInfo();
	}
	
	//服务端反馈客户端请求
	virtual bool SendData(struct evhttp_request * req, VOID * pData, WORD wDataSize, const char * type = "txt", int respCode = HTTP_OK)
	{
		if (m_cbServerType != SERVER_TYPE_SERVER)
			return false;

		if (!req)
			return false;

		evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", type);

		struct evbuffer *pResponce = evbuffer_new();
		if (!pResponce)
		{
			evhttp_send_error(req, HTTP_INTERNAL, "got err,please try again later!");
			return false;
		}

		evbuffer_add(pResponce, pData, wDataSize);

		evhttp_send_reply(req, respCode, "", pResponce);

		evbuffer_free(pResponce);

		return true;
	}

	//作为客户端的接口
public:
	//客户端异步Get请求
	virtual bool Get(const char * url, HTTP_SERVICE_EVENT_FUNCTOR pCB, T * ptr, void * arg = NULL)
	{
		if (m_cbServerType != SERVER_TYPE_CLIENT)
			return false;

		tagClientCallBackInfo ifo;
		ifo.pObject = ptr;
		ifo.pArg = arg;
		ifo.pFunc = pCB;
		ifo.cbReqType = EVHTTP_REQ_GET;
		ifo.pData = NULL;
		ifo.nSize = 0;
		strcpy(ifo.url, url);
		m_ReqClientFunctors.push_back(ifo);

		return true;
	}
	//客户端异步Post请求
	virtual bool Post(const char * url, void * pData, unsigned short wDataSize, HTTP_SERVICE_EVENT_FUNCTOR pCB, T * ptr, void * arg = NULL)
	{
		if (m_cbServerType != SERVER_TYPE_CLIENT)
			return false;

		tagClientCallBackInfo ifo;
		ifo.pFunc = pCB;
		ifo.pObject = ptr;
		ifo.pArg = arg;
		ifo.cbReqType = EVHTTP_REQ_POST;
		strcpy(ifo.url, url);
		ifo.pData = malloc(wDataSize);
		ifo.nSize = wDataSize;
		memcpy(ifo.pData, pData, wDataSize);
		m_ReqClientFunctors.push_back(ifo);

		return true;
	}

protected:
	//处理客户端请求
	bool ProcessRequest(unsigned char cbReqType, const char * url, void * pData, unsigned short wDataSize)
	{
		if (m_cbServerType != SERVER_TYPE_CLIENT)
			return false;

		//变量申明
		struct evhttp_uri *http_uri = NULL;
		const char *scheme, *host, *path, *query;
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		bool ret = true;

		do
		{
			http_uri = evhttp_uri_parse(url);
			if (http_uri == NULL) {
				ret = false;
				break;
			}

			bool isHttps = false;

			scheme = evhttp_uri_get_scheme(http_uri);


			if (scheme)
			{
				strcpy(buffer, scheme);

				for (int i = 0; i < strlen(scheme); ++i)  
				{  
					buffer[i] = tolower(buffer[i]);  
				}
			}

			if (strcmp(buffer, "https") != 0 && strcmp(buffer, "http") != 0)
			{
				ret = false;
				break;
			}

			if (strcmp(buffer, "https") == 0)
			{
				isHttps = true;
			}

			host = evhttp_uri_get_host(http_uri);
			if (host == NULL) {
				printf("url must have a host \n");
				ret = false;
				break;
			}

			int port = evhttp_uri_get_port(http_uri);
			if (port == -1) {
				port = isHttps ? 443 : 80;
			}

			path = evhttp_uri_get_path(http_uri);
			if (strlen(path) == 0) {
				path = "/";
			}

			char uri[512] = { 0 };

			query = evhttp_uri_get_query(http_uri);
			if (query == NULL) {
				sprintf(uri, "%s", path);
			}
			else {
				sprintf(uri, "%s?%s", path, query);
			}
			uri[sizeof(uri) - 1] = '\0';

			//-------we do not verify peer--------//
			//like the curl SSL_VERIFYPEER is set false

			//if (1 != SSL_CTX_load_verify_locations(ssl_ctx, crt, NULL)) {
			//	err_openssl("SSL_CTX_load_verify_locations");
			//	goto error;
			//}
			//SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
			//SSL_CTX_set_cert_verify_callback(ssl_ctx, cert_verify_callback,(void *)host);

			struct bufferevent* bev = NULL;


#if HTTP_KERNEL_ENABLE_SSL
			SSL *pSSL = SSL_new(m_pSSLCtx);
			if (pSSL == NULL) {
				printf("SSL_new err.");
				ret = false;
				break;
			}
#endif

			if (!isHttps) {
				bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
			}
			else {
#if HTTP_KERNEL_ENABLE_SSL
				bev = bufferevent_openssl_socket_new(base, -1, pSSL,
					BUFFEREVENT_SSL_CONNECTING,
					BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
#endif
			}

			if (bev == NULL) {
				fprintf(stderr, "bufferevent_socket_new() failed\n");
				ret = false;
				break;
			}

#if HTTP_KERNEL_ENABLE_SSL
			if (isHttps)
			{
				bufferevent_openssl_set_allow_dirty_shutdown(bev, 1);
			}
#endif
			struct evhttp_connection* evcon = evhttp_connection_base_bufferevent_new(base, NULL, bev, host, port);
			if (evcon == NULL) {
				fprintf(stderr, "evhttp_connection_base_bufferevent_new() failed\n");
				ret = false;
				break;
			}

			if (m_nRetry > 0) {
				evhttp_connection_set_retries(evcon, m_nRetry);
			}
			if (m_nTimeOut >= 0) {
				evhttp_connection_set_timeout(evcon, m_nTimeOut);
			}

			//HttpObject* pHttpObj = new HttpObject(this, bev, strUserData, pCB);

			// Fire off the request
			struct evhttp_request* req = evhttp_request_new(listener_gencb, this);
			if (req == NULL) {
				fprintf(stderr, "evhttp_request_new() failed\n");
				ret = false;
				break;
			}

			struct evkeyvalq *output_headers = evhttp_request_get_output_headers(req);
			evhttp_add_header(output_headers, "Host", host);
			evhttp_add_header(output_headers, "Connection", "close");

			if (wDataSize>0)
			{
				struct evbuffer *output_buffer = evhttp_request_get_output_buffer(req);
				evbuffer_add(output_buffer, pData, wDataSize);
				char buf[256] = { 0 };
				evutil_snprintf(buf, sizeof(buf) - 1, "%lu", (unsigned long)wDataSize);
				evhttp_add_header(output_headers, "Content-Length", buf);
			}

			int r_ = evhttp_make_request(evcon, req, (evhttp_cmd_type)cbReqType, uri);
			if (r_ != 0) {
				fprintf(stderr, "evhttp_make_request() failed\n");
				ret = false;
				break;
			}
		}
		while(0);

		if (http_uri)
		{
			evhttp_uri_free(http_uri);
		}

		return ret;
	}

protected:
	//通用回调函数
	static void listener_gencb(struct evhttp_request *req, void *ctx)
	{
		CHttpKernel* pNetKernel = (CHttpKernel*)ctx;

		if (req == NULL)
		{
			if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
			{
				tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
				pNetKernel->m_ReqClientFunctors.pop_front();

				if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
					(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(NULL, pCallBackInfo.pArg);

				if (pCallBackInfo.pData)
					free(pCallBackInfo.pData);

				pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
			}
			else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
			{
				tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqGetCallBackInfo();
				if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
					(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(NULL, pCallBackInfo.pArg);
			}
			else
			{
				printf("error!");
			}

			return;
		}

		enum evhttp_cmd_type eCmdType = evhttp_request_get_command(req);
		switch (eCmdType)
		{
		case EVHTTP_REQ_GET:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();

					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);

					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqGetCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}

			}
			break;
		case EVHTTP_REQ_POST:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqPostCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_HEAD:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqHeadCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_PUT:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqPutCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_DELETE:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqDeleteCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_OPTIONS:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqOptionsCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_TRACE:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqTraceCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_CONNECT:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqConnectCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		case EVHTTP_REQ_PATCH:
			{
				if (pNetKernel->m_cbServerType == SERVER_TYPE_CLIENT)
				{
					tagClientCallBackInfo pCallBackInfo = pNetKernel->m_ReqClientFunctors.front();
					pNetKernel->m_ReqClientFunctors.pop_front();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);

					if (pCallBackInfo.pData)
						free(pCallBackInfo.pData);
					pNetKernel->m_cbClientStates = CLIENT_STATE_FREE;
				}
				else if (pNetKernel->m_cbServerType == SERVER_TYPE_SERVER)
				{
					tagServerCallBackInfo pCallBackInfo = pNetKernel->GetReqPatchCallBackInfo();
					if (pCallBackInfo.pObject && pCallBackInfo.pFunc)
						(pCallBackInfo.pObject->*pCallBackInfo.pFunc)(req, pCallBackInfo.pArg);
				}
				else
				{
					printf("error!");
				}
			}
			break;
		default:
			assert(false);
			break;
			
		}
	}

private:
	int								mHttpPort;
	struct event_base*				base;
	struct evhttp*					http;
	unsigned char					m_cbServerType;

#if HTTP_KERNEL_ENABLE_SSL
	SSL_CTX *						m_pSSLCtx;
#endif
	int								m_nRetry;
	int								m_nTimeOut;
	//HTTP服务器事件回调列表
	ServerCallBackInfo				m_ReqServerFunctors;
	//HTTP客户端事件回调列表
	ClientCallBackInfo				m_ReqClientFunctors;
	unsigned char					m_cbClientStates;
};

#endif
