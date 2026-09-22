"""Compile and exercise the actual template. Set CP_TEMPLATE_CXX=cl for MSVC."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]

HARNESS = r'''
#define main cp_template_main
#include "TEMPLATE_PATH"
#undef main

struct Node { int u, v; };
std::ostream& operator<<(std::ostream& out, const Node& node) {
    return out << "Node(" << node.u << ", " << node.v << ')';
}

template<typename T>
std::string render(T&& value) {
    std::ostringstream buffer;
    auto* previous = std::cerr.rdbuf(buffer.rdbuf());
    debug(std::forward<T>(value));
    std::cerr.rdbuf(previous);
    auto result = buffer.str();
    const auto start = result.find("] = [");
    assert(start != std::string::npos && result.ends_with("]\n"));
    return result.substr(start + 5, result.size() - start - 7);
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
#if defined(ONLINE_JUDGE) || defined(NO_DEBUG)
    struct Unprintable {};
    int side_effects = 0;
    if (true) debug(++side_effects, Unprintable{}); else return 1;
    debug_matrix(++side_effects);  // Even invalid debug-only types must disappear.
    debug();
    assert(side_effects == 0);
    cout << "disabled";
#else
    assert(argc == 2);
    const std::string mode = argv[1];
    if (mode == "nested") {
        assert(render(vi{1, 2}) == "{1, 2}");
        assert(render(pr<int, vi>{1, {2, 3}}) == "{1, {2, 3}}");
        assert(render(tuple<vi, optional<int>>{{4, 5}, 6}) == "({4, 5}, Some(6))");
        assert(render(vec<optional<vi>>{{vi{1, 2}}, nullopt}) == "{Some({1, 2}), None}");
        assert(render(map<int, vp>{{1, {{2, 3}}}}) == "{{1, {{2, 3}}}}");
        assert(render(tuple<>{}) == "()");
        assert(render(vii{{}, {1, 2}}) == "{{}, {1, 2}}");
        assert(render(variant<int, vi>{vi{3, 4}}) == "Variant({3, 4})");
        assert(render(variant<monostate, int>{}) == "Variant(monostate)");
        assert(render(Node{1, 2}) == "Node(1, 2)");
        assert(render(pair<Node, vi>{{3, 4}, {5}}) == "{Node(3, 4), {5}}");
    } else if (mode == "ranges") {
        assert(render(array<int, 3>{1, 2, 3}) == "{1, 2, 3}");
        int raw[] = {1, 2};
        assert(render(raw) == "{1, 2}");
        assert(render(span<const int>{raw}) == "{1, 2}");
        assert(render(deque<int>{1, 2}) == "{1, 2}");
        assert(render(list<int>{3, 4}) == "{3, 4}");
        assert(render(set<int>{3, 1}) == "{1, 3}");
        assert(render(multiset<int>{3, 1, 1}) == "{1, 1, 3}");
        assert(render(unordered_set<int>{7}) == "{7}");
        assert(render(unordered_map<int, vi>{{7, {8}}}) == "{{7, {8}}}");
        vector<bool> bits{true, false};
        assert(render(bits) == "{true, false}");
        assert(render(bits[0]) == "true");
        assert(render(bitset<4>{5}) == "0101");
        auto even = views::iota(1, 7) | views::filter([](int x) { return x % 2 == 0; });
        assert(render(even) == "{2, 4, 6}");
        assert(render(string("a\n\"b\\", 5)) == "\"a\\n\\\"b\\\\\"");
        assert(render('\n') == "'\\n'");
        assert(render('\'') == "'\\''");
        assert(render("hi") == "\"hi\"");
        char bounded[] = {'x', 'y'};
        assert(render(bounded) == "\"xy\"");
        const char* missing = nullptr;
        assert(render(missing) == "nullptr");
        assert(render(string_view("abcd").substr(1, 2)) == "\"bc\"");
    } else if (mode == "adaptors") {
        queue<vi> q;
        q.push({1, 2}); q.push({3});
        assert(render(q) == "{{1, 2}, {3}}");
        assert(q.size() == 2 && q.front() == vi({1, 2}));
        const auto cq = q;
        assert(render(cq) == "{{1, 2}, {3}}");
        stack<pii> s;
        s.push({1, 2}); s.push({3, 4});
        assert(render(s) == "{{3, 4}, {1, 2}}");
        assert(s.size() == 2 && s.top() == pii(3, 4));
        priority_queue<int> big;
        minpq<int> small;
        for (int x : {3, 1, 2}) { big.push(x); small.push(x); }
        assert(render(big) == "{3, 2, 1}");
        assert(render(small) == "{1, 2, 3}");
        assert(big.size() == 3 && big.top() == 3 && small.top() == 1);
        assert(render(queue<int>{}) == "{}");
    } else if (mode == "macros") {
        vi a{3, 1, 2};
        sort(all(a));
        assert(a == vi({1, 2, 3}) && sz(a) == 3);
        sort(rall(a));
        a.pb(4);
        vp edges;
        edges.eb(5, 6);
        assert(edges[0].fi == 5 && edges[0].se == 6);
        int raw[] = {3, 2, 1};
        sort(all(raw));
        assert(raw[0] == 1 && sz(raw) == 3);
        int bounds_called = 0;
        auto upper = [&] { ++bounds_called; return 3; };
        vi forward, backward;
        rep(i, 0, upper()) forward.pb(i);
        per(i, 0, upper()) backward.pb(i);
        assert(forward == vi({0, 1, 2}) && backward == vi({2, 1, 0}));
        assert(bounds_called == 2);
        rep(i, 0, 0) assert(false);
        per(i, 0, 0) assert(false);
        int best = 10;
        assert(chmin(best, 4) && !chmin(best, 5));
        assert(chmax(best, 8) && !chmax(best, 7) && best == 8);
        ostringstream answer;
        auto* previous = cout.rdbuf(answer.rdbuf());
        print(vi{1, 2, 3});
        print(raw, ',');
        print(vi{});
        cout.rdbuf(previous);
        assert(answer.str() == "1 2 3\n1,2,3\n\n");
        ostringstream log;
        previous = cerr.rdbuf(log.rdbuf());
        int side_effects = 0;
        if (true) debug(++side_effects, a); else return 1;
        debug();
        debug_matrix(vii{{1, 2}, {3, 4}});
        cerr.rdbuf(previous);
        assert(side_effects == 1);
        assert(log.str().find("[++side_effects, a] = [1, {3, 2, 1, 4}]") != string::npos);
        assert(log.str().find("[\n  {1, 2}\n  {3, 4}\n]\n") != string::npos);
    } else return 2;
    cout << "ok";
#endif
}
'''


class TemplateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.compiler = os.environ.get("CP_TEMPLATE_CXX", "g++")
        if not shutil.which(cls.compiler):
            raise unittest.SkipTest(f"Compiler not on PATH: {cls.compiler}")
        parent = ROOT / "build/template-tests"
        parent.mkdir(parents=True, exist_ok=True)
        cls.temporary = tempfile.TemporaryDirectory(prefix="case-", dir=parent)
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.folder = Path(cls.temporary.name).resolve()
        assert cls.folder.is_relative_to(ROOT)
        cls.compiled = {}
        template = ROOT / "templates/main.cpp"
        relative = os.path.relpath(template, cls.folder).replace("\\", "/")
        cls.harness = HARNESS.replace("TEMPLATE_PATH", relative)
        source = template.read_text(encoding="utf-8")
        stub = "void solve() {\n    // Write your solution here.\n}"
        assert source.count(stub) == 1
        cls.instrumented = source.replace(stub, r'''void solve() {
    long long x;
    if (cin >> x) cout << x * 2 << '\n';
}''')

    @classmethod
    def build(cls, name, flags=(), instrumented=False):
        if name not in cls.compiled:
            source = cls.folder / f"{name}.cpp"
            executable = cls.folder / f"{name}.exe"
            source.write_text(cls.instrumented if instrumented else cls.harness, encoding="utf-8")
            relative_source = str(source.relative_to(ROOT))
            relative_executable = str(executable.relative_to(ROOT))
            if Path(cls.compiler).stem.lower() == "cl":
                command = [cls.compiler, "/nologo", "/std:c++20", "/EHsc", "/utf-8", "/W4",
                           "/permissive-", "/D_CRT_SECURE_NO_WARNINGS", "/DLOCAL",
                           *[f"/D{flag}" for flag in flags], relative_source,
                           f"/Fe{relative_executable}", f"/Fo{source.with_suffix('.obj').relative_to(ROOT)}"]
            else:
                command = [cls.compiler, "-std=c++20", "-Wall", "-Wextra", "-pedantic", "-DLOCAL",
                           *[f"-D{flag}" for flag in flags], relative_source, "-o", relative_executable]
            result = subprocess.run(command, cwd=ROOT, capture_output=True, timeout=90)
            if result.returncode:
                raise AssertionError((result.stdout + result.stderr).decode("utf-8", errors="replace"))
            cls.compiled[name] = executable
        return cls.compiled[name]

    def run_binary(self, executable, *args, input=b""):
        result = subprocess.run([str(executable), *args], cwd=self.folder,
                                input=input, capture_output=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr.decode("utf-8", errors="replace"))
        self.assertEqual(result.stderr, b"")
        return result.stdout.replace(b"\r\n", b"\n")

    def test_nested_containers_optional_variant_and_custom_struct(self):
        self.assertEqual(self.run_binary(self.build("normal"), "nested"), b"ok")

    def test_ranges_strings_and_boolean_proxies(self):
        self.assertEqual(self.run_binary(self.build("normal"), "ranges"), b"ok")

    def test_adaptors_are_printed_in_pop_order_without_modification(self):
        self.assertEqual(self.run_binary(self.build("normal"), "adaptors"), b"ok")

    def test_macros_print_matrix_and_if_else(self):
        self.assertEqual(self.run_binary(self.build("normal"), "macros"), b"ok")

    def test_online_judge_removes_debug_and_argument_evaluation(self):
        self.assertEqual(self.run_binary(self.build("judge", ("ONLINE_JUDGE",))), b"disabled")

    def test_no_debug_removes_debug_and_argument_evaluation(self):
        self.assertEqual(self.run_binary(self.build("quiet", ("NO_DEBUG",))), b"disabled")

    def test_single_and_multiple_case_switch(self):
        self.assertEqual(self.run_binary(self.build("single", instrumented=True), input=b"7\n"), b"14\n")
        self.assertEqual(self.run_binary(self.build("multi", ("MULTI_CASE_INPUT",), True),
                                         input=b"3\n4 -5 6\n"), b"8\n-10\n12\n")

    def test_local_file_io(self):
        (self.folder / "data").mkdir(exist_ok=True)
        (self.folder / "data/input.txt").write_bytes(b"9\n")
        self.assertEqual(self.run_binary(self.build("file", ("LOCAL_FILE",), True), input=b"88\n"), b"")
        result = (self.folder / "data/output.txt").read_bytes().replace(b"\r\n", b"\n")
        self.assertEqual(result, b"18\n")


if __name__ == "__main__":
    unittest.main()
