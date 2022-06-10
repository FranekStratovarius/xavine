set_project("xavine")
add_rules("mode.debug", "mode.release")
add_repositories("xavine-xrepo https://github.com/FranekStratovarius/xmake-repo master")
add_requires("bgfx 7816", "flecs v3.0.1-alpha", "glfw 3.3.5", {system = false})
add_requireconfs("bgfx")
add_requireconfs("flecs", {configs = {shared = true}})
add_requireconfs("glfw", {configs = {shared = true}})
includes("modules/graphics/xmake.lua")

target("xavine") do
	set_kind("binary")
	set_languages("cxx11")

	add_files("src/main.cpp")
	add_includedirs("include")

	set_warnings("all")
	set_optimize("fastest")

	add_packages("bgfx", "flecs", "glfw")
	add_deps("xavine-graphics")

	-- copy asset folder after build
	after_build(function (target)
		-- copy assets to build output folder
		os.cp(path.join("assets"), path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)"))
	end)

	--[[
	on_package(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "packages", "x", "xavine", "$(os)", "$(arch)", "$(mode)", "bin"))
	end)
	--]]
end