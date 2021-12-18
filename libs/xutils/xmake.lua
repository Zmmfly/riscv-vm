set_target("xutils")
    set_kind("static")
    add_cfiles("src/*.cc")
    add_includedirs("include", {public=true})
