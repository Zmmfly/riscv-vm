includes("libs")

target("riscv-vm")
    set_kind("binary")
    add_deps("xutils", "rvvm")
    add_includedirs("include")
    add_files("src/*.cc")
    add_files("src/ins/*.cc")
