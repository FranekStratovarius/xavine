set_project("xavine")
add_rules("mode.debug", "mode.release")
add_repositories("xavine-xrepo https://github.com/FranekStratovarius/xmake-repo master")
add_requires("glfw 3.3.5", "bgfx 7816", "imgui v1.87-docking", "flecs v3.0.1-alpha", {system = false})

target("xavine") do
	set_kind("binary")
	set_languages("cxx11")

	add_files("src/main.cpp")
	add_includedirs("include")

	set_warnings("all")
	set_optimize("fastest")

	-- set bgfx platform defines
	if is_plat("linux") then
		add_syslinks("dl")
		add_defines("BX_PLATFORM_LINUX")
	elseif is_plat("windows") then
		add_defines("BX_PLATFORM_WINDOWS")
	elseif is_plat("macosx") then
		add_defines("BX_PLATFORM_OSX")
	end

	add_packages("glfw", "imgui", "bgfx", "flecs")

	-- copy asset folder after build
	after_build(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)"))
	end)

	on_package(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "packages", "x", "xavine", "$(os)", "$(arch)", "$(mode)", "bin"))
	end)
end
