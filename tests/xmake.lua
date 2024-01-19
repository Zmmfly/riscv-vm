add_requires("gtest")
for _, file in ipairs(os.files("src/rv32/test_*.cc")) do
    local name = path.basename(file)
    target(name)
        set_kind("binary")
        set_default(false)
        set_languages("c++20")
        add_deps("rvvm")
        add_files("src/rv32/" .. name .. ".cc")
        add_tests("default")
        add_packages("gtest")
end
