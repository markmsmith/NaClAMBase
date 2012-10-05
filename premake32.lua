local nacl_toolchain_path = os.getenv("NACL_SDK_BIN")
premake.gcc.cc  = nacl_toolchain_path .. "/i686-nacl-gcc"
premake.gcc.cxx = nacl_toolchain_path .. "/i686-nacl-g++"
premake.gcc.ar  = nacl_toolchain_path .. "/i686-nacl-ar"

solution "NaClAM"
	configurations { "Debug", "Release" }

project "NaClAMBase"
	kind "StaticLib"
	language "C++"
	files { 
		"NaClAMBase/*.cpp",
		}
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "premake/lib/debug"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m32 -msse2" }
	configuration "Release"
		defines {"NDEBUG"}
		flags {"Optimize"}
		targetdir "premake/lib/release"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m32 -msse2" }

project "NaClAMTest"
	kind "ConsoleApp"
	language "C++"
	files {
		"NaClAMTest/*.cpp",
		}
	links { "NaClAMBase", "m", "pthread", "ppapi" }
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "premake/bin/debug"
		targetextension ".nexe"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m32 -msse2" }
	configuration "Release"
		defines {"NDEBUG"}
		flags {"Optimize"}
		targetdir "premake/bin/release"
		targetextension ".nexe"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m32 -msse2" }
