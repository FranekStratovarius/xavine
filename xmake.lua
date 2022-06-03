set_project("xavine")
add_rules("mode.debug", "mode.release")
add_requires("glfw 3.3.5", "bgfx 7816","imgui v1.87-docking",{system = false})

target("xavine") do
	set_kind("binary")

	add_files("src/main.cpp")
	add_includedirs("include")
	-- add bgfx compat include path
	if is_plat("windows") then
		add_includedirs("C:\\Users\\louis\\AppData\\Local\\.xmake\\packages\\b\\bgfx\\7816\\e024fd36069a4b5b83561fec3bb8fd07\\include\\compat\\msvc")
	end

	on_load(function (target)
        import("lib.detect.find_library")
        local package = find_library("bgfx")
		print("bgfx path:", package.includedir)
    end)

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

	add_packages("glfw", "imgui", "bgfx")

	-- copy asset folder after build
	after_build(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)"))
	end)
end
