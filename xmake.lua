add_rules("mode.debug", "mode.release")
add_repositories("xavine-xrepo https://github.com/FranekStratovarius/xmake-repo master")
add_requires("bgfx 7816", "flecs v3.0.1-alpha", "glfw 3.3.5", {system = false})
add_requireconfs("bgfx", {configs = {shared = false}})
add_requireconfs("flecs", {configs = {shared = false}})
add_requireconfs("glfw", {configs = {shared = false}})

rule("shader") do
	set_extensions(".sc")
	on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
		-- list of shader models
		local shader_models = {
			-- OpenGL ES Shading Language / WebGL (ESSL)
			{lang = "essl", id = "320_es"},
			-- Metal Shading Language (MSL)
			{lang = "metal", id = "metal"},
			-- Standard Portable Intermediate Representation - V (SPIR-V) [Vulkan]
			{lang = "spirv", id = "spirv15-12"},
			-- OpenGL Shading Language (GLSL)
			{lang = "glsl", id = "440"},
		}

		-- High-Level Shading Language (HLSL) [DirectX] only on windows
		if is_plat("windows") then
			table.insert(shader_models, {lang = "dx9",  id = "s_3_0", dx = true})
			table.insert(shader_models, {lang = "dx11", id = "s_4_0", dx = true})
			table.insert(shader_models, {lang = "dx12", id = "s_5_0", dx = true})
		end

		-- compile shaders and output them in the build output folder
		for _, shader_model in ipairs(shader_models) do
			-- make sure build directory exists
			batchcmds:mkdir(path.join(target:targetdir(), "assets", "shaders", shader_model.lang))

			-- get type of shader
			local shader_type = string.sub(path.basename(sourcefile), 1, 2)
			local shader_tags = nil
			if shader_type == "fs" then
				shader_tags = {long = "fragment", short = "p"}
			elseif shader_type == "vs" then
				shader_tags = {long = "vertex  ", short = "v"}
			elseif shader_type == "cs" then
				shader_tags = {long = "compute ", short = "c"}
			else
				return
			end

			-- compile shader
			local targetfile = path.join(target:targetdir(), "assets", "shaders", shader_model.lang, path.basename(sourcefile)..".bin")
			batchcmds:vrunv(
				path.join(target:pkgs()["bgfx"]:installdir(), "bin", "shadercRelease")
				.." -f "..sourcefile
				.." -o "..targetfile
				..vformat(" --platform $(os)")
				.." --profile "..(shader_model.dx and shader_tags.short or "")..shader_model.id
				.." --type "..shader_tags.long
				..vformat(" --varyingdef "..path.join("$(projectdir)", "shaders", "varying.def.sc"))
				--..vformat(" --verbose")
				..vformat(" -i "..path.join("$(projectdir)", "shaders", "src"))
			)
			batchcmds:show_progress(opt.progress, "${color.build.object}compiling "..shader_tags.long.." shader %s to %s", sourcefile, targetfile)
		end	-- shader_model

		-- only rebuild the file if its changed since last run
		batchcmds:add_depfiles(sourcefile)
    end)
end

target("xavine") do
	set_kind("binary")
	set_languages("cxx11")

	add_files("src/**.cpp")
	add_includedirs("include",{public=true})

	set_warnings("all")
	set_optimize("fastest")

	-- if on windows, use msvc2019
	if is_plat("windows") then
		target:set("toolchains", "msvc", {vs = "2019"})
	end

	-- set bgfx platform defines
	if is_plat("linux") then
		add_syslinks("dl")
		add_defines("BX_PLATFORM_LINUX")
	elseif is_plat("windows") then
		add_defines("BX_PLATFORM_WINDOWS")
	elseif is_plat("macosx") then
		add_defines("BX_PLATFORM_OSX")
	end

	add_rules("shader")
	add_files("shaders/**.sc")

	add_packages("bgfx", "flecs", "glfw")

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