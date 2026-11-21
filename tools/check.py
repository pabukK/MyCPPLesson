"""检查课程里的示例代码:提取、编译、运行。
用法: python tools/check.py [课文文件名前缀] [-v]
代码块第一行写 `// file: 名字.cpp` 才会被检查。
可选:在代码块前一行放 HTML 注释,每行 key: value,课文里看不到:
  flags: 额外编译选项      stdin: 输入内容(\\n 表示换行)
  expect: ok|crash|compile-fail
"""
import re, subprocess, sys, pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
OUT = pathlib.Path(r"C:\Users\kevin\AppData\Local\Temp\opencode\cppcheck")
BLOCK = re.compile(r"(?:<!--(.*?)-->\s*\n)?```(cpp|cmake)\n(.*?)```", re.S)
HEAD = re.compile(r"^(?://|#) (\w+): ?(.*)$")


def wsl_path(p):
    d = str(p).replace("\\", "/")
    return "/mnt/" + d[0].lower() + d[2:]


def parse(block):
    meta = {}
    for line in block.split("\n"):
        m = HEAD.match(line)
        if not m:
            break
        meta[m.group(1)] = m.group(2).strip()
    return meta


def run_wsl(cmd, timeout=120):
    r = subprocess.run(["wsl", "-d", "Ubuntu-24.04", "--", "bash", "-c", cmd],
                       capture_output=True, timeout=timeout)
    return r.returncode, r.stdout.decode("utf-8", "replace")

def check_file(md, verbose):
    text = md.read_text(encoding="utf-8")
    work = OUT / md.stem
    work.mkdir(parents=True, exist_ok=True)
    bad = 0
    for comment, lang, body in BLOCK.findall(text):
        meta = parse(body)
        for line in comment.split("\n"):
            k, _, v = line.partition(":")
            if v:
                meta[k.strip()] = v.strip()
        if lang != "cpp" or "file" not in meta:
            continue
        name = meta["file"]
        target = work / name
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(body, encoding="utf-8", newline="\n")
        if name.endswith(".h") or meta.get("nobuild"):
            continue
        exe = name.rsplit(".", 1)[0]
        flags = meta.get("flags", "")
        sources = meta.get("sources", name)
        d = wsl_path(work)
        build = f"cd {d} && g++ -std=c++17 -Wall -Wextra {flags} -o {exe} {sources} 2>&1"
        code, out = run_wsl(build)
        expect = meta.get("expect", "ok")
        if expect == "compile-fail":
            ok = code != 0
        elif code != 0:
            ok, out = False, out
        else:
            stdin = meta.get("stdin", "").replace("\\n", "\n")
            (work / (exe + ".in")).write_text(stdin, encoding="utf-8", newline="\n")
            code, out = run_wsl(f"cd {d} && timeout 10 ./{exe} < {exe}.in 2>&1")
            ok = (code == 0) if expect == "ok" else (code != 0)
        print(("PASS " if ok else "FAIL ") + md.name + " " + name)
        if verbose or not ok:
            print(out)
        bad += 0 if ok else 1
    return bad

def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    verbose = "-v" in sys.argv
    files = sorted((ROOT / "lessons").glob("*.md"))
    if args:
        files = [f for f in files if f.name.startswith(args[0])]
    bad = sum(check_file(f, verbose) for f in files)
    print("全部通过" if bad == 0 else f"有 {bad} 个示例失败")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
