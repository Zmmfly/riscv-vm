target("rvvm")
    set_kind("static")
    add_files("src/**.cc")
    add_includedirs("inc", {public=true})
    set_languages("c++20")
    set_warnings("all", "error")

includes("tests")
