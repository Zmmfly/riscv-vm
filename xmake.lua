includes("libs")

target("riscv-vm")
    set_kind("binary")
    add_deps("xutils")
    add_includedirs("include")
    add_files("src/*.cc")