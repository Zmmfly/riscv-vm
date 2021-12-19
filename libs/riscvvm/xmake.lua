target("riscvvm")
    set_kind("static")
    add_files("src/*.cc")
    add_includedirs("include", {public=true})