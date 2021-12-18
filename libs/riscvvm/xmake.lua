set_target("riscvvm")
    set_target("static")
    add_cfiles("src/*.cc")
    add_includedirs("include", {public=true})