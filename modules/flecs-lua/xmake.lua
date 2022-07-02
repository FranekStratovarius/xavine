add_rules("mode.debug", "mode.release")
if is_plat("macosx") then
	-- use static libs on macosx
	add_requires("flecs v3.0.1-alpha", "luajit 2.1.0-beta3", {system = false})
else
	add_requires("flecs v3.0.1-alpha", "luajit 2.1.0-beta3", {system = false, configs = {shared = true}})
end

target("flecs-lua") do
	set_kind("static")
	set_optimize("fastest")
	add_files("src/**.c")
	--add_files("src/**.cpp")
	add_includedirs("include",{public=true})

	if not is_plat("windows") then
		set_languages("gnuxxlatest")
	end

	if is_plat("macosx") then
		-- link statically on macosx
		add_packages("flecs", "luajit")
	else
		add_packages("flecs", {links = "flecs"})
		add_packages("luajit", {links = "luajit"})
		-- add folder of executable to LD_LIBRARY_PATH
		add_rpathdirs(".")
	end

	-- copy libs after build
	after_build(function (target)
		-- copy dynamic libs to build output folder
		if not is_plat("macosx") then
			-- only copy dynamic libs if not in macosx
			for _, package in ipairs({
				{name = "luajit", libname = "luajit"},
				{name = "flecs", libname = "flecs"},
			}) do
				os.cp(
					path.join(target:pkgs()[package.name]:installdir(), (is_plat("windows") and "bin" or "lib"), "*"..package.libname..(is_plat("windows") and ".dll" or ".so")),
					path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")
				)
			end
		end
	end)
end