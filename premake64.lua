local nacl_toolchain_path = os.getenv("NACL_SDK_BIN")
premake.gcc.cc  = nacl_toolchain_path .. "/x86_64-nacl-gcc"
premake.gcc.cxx = nacl_toolchain_path .. "/x86_64-nacl-g++"
premake.gcc.ar  = nacl_toolchain_path .. "/x86_64-nacl-ar"

solution "NaClAM"
	configurations { "Debug", "Release" }

project "NaClAMBase64"
	kind "StaticLib"
	language "C++"
	files { 
		"NaClAMBase/*.cpp",
		}
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "premake/lib/debug"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64 -msse2" }
	configuration "Release"
		defines {"NDEBUG"}
		flags {"Optimize"}
		targetdir "premake/lib/release"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64 -msse2" }

project "NaClAMTest64"
	kind "ConsoleApp"
	language "C++"
	files {
		"NaClAMTest/*.cpp",
		}
	includedirs { "." }
	links { "NaClAMBase64", "m", "pthread", "ppapi" }
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "premake/bin/debug"
		targetextension ".nexe"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64 -msse2" }
	configuration "Release"
		defines {"NDEBUG"}
		flags {"Optimize"}
		targetdir "premake/bin/release"
		targetextension ".nexe"
		buildoptions { "-std=gnu++0x -fno-rtti -fno-exceptions -m64 -msse2" }

