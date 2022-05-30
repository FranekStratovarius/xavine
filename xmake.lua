set_project("xavine")
add_rules("mode.debug", "mode.release")
--includes("graphics/xmake.lua")
add_requires("glfw 3.3.5", "bgfx 7816","imgui v1.87-docking"--[[,"luajit 2.1.0-beta3"]],{system = false})

target("xavine") do
	set_kind("binary")
	--add_deps("xavine-graphics")



	add_files("src/main.cpp")
	add_files("src/**.cpp")
	add_includedirs("include")

	set_warnings("error")
	set_optimize("fastest")

	if is_plat("linux") then
		add_syslinks("dl")
		add_defines("BX_PLATFORM_LINUX")
	elseif is_plat("windows") then
		add_defines("BX_PLATFORM_WINDOWS")
	elseif is_plat("macosx") then
		add_defines("BX_PLATFORM_OSX")
	end
	--add_packages("glfw"--[[,"imgui","luajit"]],"xavine_graphics")
	add_packages("glfw", "imgui", "bgfx")

	-- copy asset folder after build
	after_build(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)"))
	end)
end