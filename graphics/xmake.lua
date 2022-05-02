add_rules("mode.debug", "mode.release")
add_requires("glfw 3.3.5",{system = false}--[[,"imgui v1.85",{system = false}]])

target("xavine-graphics")
	set_kind("shared")

	add_files("src/*.c")
	add_files("src/*.cpp")
	add_includedirs("include",{public=true})
	add_includedirs("include/imgui",{public=true})

	add_packages("glfw",{system = false}--[[,"imgui]])
