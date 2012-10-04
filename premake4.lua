local nacl_toolchain_path = os.getenv("NACL_SDK_BIN")
premake.gcc.cc  = nacl_toolchain_path .. "/x86_64-nacl-gcc"
premake.gcc.cxx = nacl_toolchain_path .. "/x86_64-nacl-g++"
premake.gcc.ar  = nacl_toolchain_path .. "/x86_64-nacl-ar"

solution "NaClAM"
	configurations { "Debug", "Release" }

project "pal"
        kind "StaticLib"
        language "C++"
        files {
                "pal/libpal/*.cpp",
                "pal/libpal/dlmalloc/*.cpp",
                "pal/libpal/dlmalloc/*.h",
        }
        includedirs { "pal" }
        configuration "Debug"
                defines { "DEBUG" }
                flags { "Symbols" }
                targetdir "premake/lib/debug"
                buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64" }
        configuration "Release"
                defines {"NDEBUG"}
                flags {"Optimize"}
                targetdir "premake/lib/release"
                buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64" }

project "NaClAMBase"
	kind "StaticLib"
	language "C++"
	files { 
		"NaClAMBase/*.cpp",
		"src/nacl_bullet_instance.h",
		}
	includedirs { "pal" }
	includedirs { "external/bullet/include", "pal" }
	libdirs { "external/bullet/lib", "pal/lib/debug/" }
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "premake/lib/debug"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64" }
	configuration "Release"
		defines {"NDEBUG"}
		flags {"Optimize"}
		targetdir "premake/lib/release"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64" }
