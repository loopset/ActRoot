#ifndef ROOT_RConfigure
#define ROOT_RConfigure

/* Configurations file for linuxx8664gcc */

#define R__HAVE_CONFIG

#ifdef R__HAVE_CONFIG
#define ROOTPREFIX    "/usr"
#define ROOTBINDIR    "/usr/bin"
#define ROOTLIBDIR    "/usr/lib/root"
#define ROOTINCDIR    "/usr/include"
#define ROOTETCDIR    "/etc/root"
#define ROOTDATADIR   "/usr/share/root"
#define ROOTDOCDIR    "/usr/share/doc/root"
#define ROOTMACRODIR  "/usr/share/root/macros"
#define ROOTTUTDIR    "/usr/share/doc/root/tutorials"
#define ROOTSRCDIR    "/usr/share/root/src"
#define ROOTICONPATH  "/usr/share/root/icons"
#define TTFFONTDIR    "/usr/share/root/fonts"
#endif

#define EXTRAICONPATH ""

#define R__HAS_SETRESUID   /**/
#define R__HAS_MATHMORE   /**/
#define R__HAS_PTHREAD    /**/
#undef R__HAS_CUDA    /**/
#define R__HAS_XFT    /**/
#undef R__HAS_COCOA    /**/
#define R__HAS_VC    /**/
#define R__HAS_VDT    /**/
#undef R__HAS_VECCORE    /**/
#undef R__USE_CXXMODULES   /**/
#undef R__USE_LIBCXX    /**/
#define R__HAS_STD_STRING_VIEW   /**/
#undef R__CUDA_HAS_STD_STRING_VIEW   /**/
#undef R__HAS_STD_EXPERIMENTAL_STRING_VIEW   /**/
#undef R__HAS_STOD_STRING_VIEW /**/
#define R__HAS_OP_EQUAL_PLUS_STRING_VIEW /**/
#define R__HAS_STD_APPLY /**/
#define R__HAS_STD_INVOKE /**/
#define R__HAS_STD_INDEX_SEQUENCE /**/
#define R__HAS_ATTRIBUTE_ALWAYS_INLINE /**/
#define R__HAS_ATTRIBUTE_NOINLINE /**/
#undef R__HAS_HARDWARE_INTERFERENCE_SIZE /**/
#define R__USE_IMT   /**/
#undef R__COMPLETE_MEM_TERMINATION /**/
#undef R__HAS_CEFWEB  /**/
#define R__HAS_QT5WEB  /**/
#undef R__HAS_DAVIX  /**/
#define R__HAS_DATAFRAME /**/
#undef R__LESS_INCLUDES /**/
#define R__HAS_TBB /**/
#undef R__HAS_ROOFIT_MULTIPROCESS /**/

#if defined(R__HAS_VECCORE) && defined(R__HAS_VC)
#ifndef VECCORE_ENABLE_VC
#define VECCORE_ENABLE_VC
#endif
#endif

#undef R__HAS_DEFAULT_LZ4  /**/
#define R__HAS_DEFAULT_ZLIB  /**/
#undef R__HAS_DEFAULT_LZMA  /**/
#undef R__HAS_DEFAULT_ZSTD  /**/
#undef R__HAS_CLOUDFLARE_ZLIB /**/

#define R__HAS_TMVACPU /**/
#undef R__HAS_TMVAGPU /**/
#undef R__HAS_CUDNN /**/
#define R__HAS_PYMVA /**/
#undef R__HAS_RMVA /**/

#define R__HAS_URING /**/

#endif
