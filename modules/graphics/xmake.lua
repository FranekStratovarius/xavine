add_rules("mode.debug", "mode.release")
add_requires("bgfx 7816", "glfw 3.3.5", {system = false})
add_requireconfs("bgfx")
add_requireconfs("glfw", {configs = {shared = true}})

target("xavine-graphics") do
	set_kind("static")
	set_languages("cxx11")

	add_files("src/*.cpp")
	add_includedirs("include",{public=true})

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

	add_packages("glfw", "bgfx",{system = false})

	after_build(function (target)-- list of shader models
		local shader_models = {
			-- OpenGL ES Shading Language / WebGL (ESSL)
			{lang = "essl", id = "320_es"},

			-- Metal Shading Language (MSL)
			{lang = "metal", id = "metal"},
			
			-- Standard Portable Intermediate Representation - V (SPIR-V) [Vulkan]
			{lang = "spirv", id = "spirv15-12"},

			-- OpenGL Shading Language (GLSL)
			--{lang = "glsl", id = 150},
			{lang = "glsl", id = "440"},
		}

		-- High-Level Shading Language (HLSL) [DirectX] only on windows
		if os.host == "windows" then
			table.append({lang = "dx9",  id = "s_3_0"})
			table.append({lang = "dx11", id = "s_4_0"})
			table.append({lang = "dx12", id = "s_5_0"})
		end

		-- compile shaders and output them in the build output folder
		for _, shader_model in ipairs(shader_models) do
			-- create output folder
			os.mkdir(vformat(path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "assets", "shaders", shader_model.lang)))
			-- compile fragment and vertex shader
			for _, shader_type in ipairs({
				{long = "fragment", short = "fs"},
				{long = "vertex", short = "vs"},
			}) do
				-- compile multiple shaders
				for _, file_path in ipairs(os.files(path.join("$(projectdir)", "shaders", shader_type.short.."_*.sc"))) do
					os.exec(
						path.join("tools", "bgfx", "$(os)", "$(arch)", "shadercRelease")
						..vformat(" -f "..file_path)
						..vformat(" -o "..path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "assets", "shaders", shader_model.lang, path.basename(file_path)..".bin"))
						..vformat(" --platform $(os)")
						..vformat(" -p "..shader_model.id)
						..vformat(" --type "..shader_type.long)
						..vformat(" --varyingdef "..path.join("$(projectdir)", "shaders", "varying.def.sc"))
						--..vformat(" --verbose")
						..vformat(" -i "..path.join("$(projectdir)", "shaders", "src"))
					)
				end	-- file_path
			end	-- shader_type
		end	-- shader_model
	end)
end