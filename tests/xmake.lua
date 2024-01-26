add_requires("gtest", "fmt")

target("test_rv32_common")
    set_kind("static")
    set_languages("c++20")
    add_deps("rvvm")

    add_files("src/tester_rv32.cc")
    add_includedirs("inc", {public=true})

for _, file in ipairs(os.files("src/rv32/test_*.cc")) do
    local name = path.basename(file)
    target(name)
        set_kind("binary")
        set_default(false)
        set_languages("c++20")
        add_deps("rvvm", "test_rv32_common")
        add_files("src/rv32/" .. name .. ".cc")
        add_includedirs("inc")
        add_tests("default")
        add_packages("gtest", "fmt")
end
