{
  "targets": [
	{
	"target_name":"binding",
	"sources":["png.c","jpeg.c","gif.c","imagesize.c","binding.c"],
	'dependencies':['./binding.gyp:libhttp','./binding.gyp:libpng','./binding.gyp:libjpeg'],
	},
	{
	 "target_name":"libhttp",
	 "type":'static_library',
	 "sources":["http.c"]
	},
	{
		"target_name": "libjpeg",
		"type": "static_library",
		'include_dirs': [
		'./third-party/jpeg/'
		],
		'direct_dependent_settings': {
			'include_dirs': [
				'./third-party/jpeg/'
			]
		},
		"sources": ["./third-party/jpeg/jaricom.c",
					"./third-party/jpeg/jcapimin.c",
					"./third-party/jpeg/jcapistd.c",
					"./third-party/jpeg/jcarith.c",
					"./third-party/jpeg/jccoefct.c",
					"./third-party/jpeg/jccolor.c",
					"./third-party/jpeg/jcdctmgr.c",
	 				"./third-party/jpeg/jchuff.c",
					"./third-party/jpeg/jcinit.c",
					"./third-party/jpeg/jcmainct.c",
					"./third-party/jpeg/jcmarker.c",
					"./third-party/jpeg/jcmaster.c",
					"./third-party/jpeg/jcomapi.c",
					"./third-party/jpeg/jcparam.c",
					"./third-party/jpeg/jcprepct.c",
					"./third-party/jpeg/jcsample.c",
					"./third-party/jpeg/jctrans.c",
					"./third-party/jpeg/jdapimin.c",
					"./third-party/jpeg/jdapistd.c",
					"./third-party/jpeg/jdarith.c",
					"./third-party/jpeg/jdatadst.c",
					"./third-party/jpeg/jdatasrc.c",
					"./third-party/jpeg/jdcoefct.c",
					"./third-party/jpeg/jdcolor.c",
					"./third-party/jpeg/jddctmgr.c",
					"./third-party/jpeg/jdhuff.c",
					"./third-party/jpeg/jdinput.c",
					"./third-party/jpeg/jdmainct.c",
					"./third-party/jpeg/jdmarker.c",
					"./third-party/jpeg/jdmaster.c",
					"./third-party/jpeg/jdmerge.c",
					"./third-party/jpeg/jdpostct.c",
					"./third-party/jpeg/jdsample.c",
					"./third-party/jpeg/jdtrans.c",
					"./third-party/jpeg/jerror.c",
					"./third-party/jpeg/jfdctflt.c",
					"./third-party/jpeg/jfdctfst.c",
					"./third-party/jpeg/jfdctint.c",
					"./third-party/jpeg/jidctflt.c",
					"./third-party/jpeg/jidctfst.c",
					"./third-party/jpeg/jidctint.c",
					"./third-party/jpeg/jquant1.c",
					"./third-party/jpeg/jquant2.c",
					"./third-party/jpeg/jutils.c",
					"./third-party/jpeg/jmemmgr.c",
					"./third-party/jpeg/jmemnobs.c" 
				]
    },
	{
		'target_name': 'libpng',
		'type': 'static_library',
		'include_dirs': [
			'./third-party/libpng',
			],
		'direct_dependent_settings': {
			'include_dirs': ['./third-party/libpng'],
		},
		'dependencies': ['./binding.gyp:zlib'],
		'libraries': ['-lm'],
	    'sources': [
					"./third-party/libpng/pngerror.c",
					"./third-party/libpng/pngget.c",
					"./third-party/libpng/pngmem.c",
					"./third-party/libpng/pngpread.c",
					"./third-party/libpng/pngread.c",
					"./third-party/libpng/pngrio.c",
					"./third-party/libpng/pngrtran.c",
					"./third-party/libpng/pngrutil.c",
					"./third-party/libpng/pngset.c",
					"./third-party/libpng/pngtrans.c",
					"./third-party/libpng/pngwio.c",
					"./third-party/libpng/pngwrite.c",
					"./third-party/libpng/pngwtran.c",
					"./third-party/libpng/pngwutil.c",
					"./third-party/libpng/png.c"
					]
	},
	{
		'target_name': 'zlib',
		'type': 'static_library',
		'include_dirs': [
		'./third-party/zlib/'
		],
		'direct_dependent_settings': {
			'include_dirs': [
				'./third-party/zlib/'
			]
		},
	    'sources': ["./third-party/zlib/adler32.c",
					"./third-party/zlib/crc32.c",
					"./third-party/zlib/deflate.c",
					"./third-party/zlib/infback.c",
					"./third-party/zlib/inffast.c",
					"./third-party/zlib/inflate.c",
					"./third-party/zlib/inftrees.c",
					"./third-party/zlib/trees.c",
					"./third-party/zlib/zutil.c",
					"./third-party/zlib/compress.c",
					"./third-party/zlib/uncompr.c",
					"./third-party/zlib/gzclose.c",
					"./third-party/zlib/gzlib.c",
					"./third-party/zlib/gzread.c",
					"./third-party/zlib/gzwrite.c"
					]
	},
  ],
  'conditions': [
    ['OS=="linux"', {
		
	}]
  ]
 }

		


