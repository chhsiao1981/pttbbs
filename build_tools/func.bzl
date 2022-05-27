def _fbs(ctx):
    srcs = ctx.attr.srcs
    outs = ctx.attr.outs

    for idx, each in enumerate(srcs):
        print(idx, dir(each), dir(each.label), each.files, each.label.name)
        out_h = ctx.actions.declare_file(each.label.name + ".h")

        ctx.actions.run_shell(
            mnemonic="FlatC",
            command="echo \"to flatc $(each.label.name)\"; flatc --cpp $(each.label.name) --filename-suffix .fbs",
            outputs=[out_h],
        )

    compilation_context = cc_common.create_compilation_context(headers=depset(outs))
    return [CcInfo(compilation_context=compilation_context)]


fbsrule = rule(
    implementation = _fbs,
    attrs = {
        "srcs": attr.label_list(allow_files = [".fbs"]),
        "outs": attr.label_list(allow_files = [".h"])
    },
)
