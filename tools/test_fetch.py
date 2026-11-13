"""Offline checks for sample fetching, the browser listener and the LeetCode driver.

CP_LEETCODE_CXX=cl uses MSVC."""
from argparse import Namespace
from contextlib import redirect_stdout
import email.message
import http.client
import io
import json
import os
from pathlib import Path
import shutil
import socket
import tempfile
import threading
import time
import unittest
from unittest.mock import patch
import urllib.error

import cp
import fetch_problem as fp

ROOT = Path(__file__).resolve().parents[1]

CODEFORCES_PAGE = """<html><body><div class="problemindexholder" problemindex="F"><div class="problem-statement">
<div class="header"><div class="title">F. Demo &amp; Test</div>
<div class="time-limit"><div class="property-title">time limit per test</div>2 seconds</div></div>
<div><p>Statement <pre>not a sample</pre></p></div>
<div class="sample-tests"><div class="section-title">Examples</div><div class="sample-test">
<div class="input"><div class="title">Input</div><pre>
<div class="test-example-line test-example-line-even test-example-line-0">2</div><div class="test-example-line test-example-line-odd test-example-line-1">1 2</div></pre></div>
<div class="output"><div class="title">Output</div><pre>
3
</pre></div>
<div class="input"><div class="title">Input</div><pre>8<br />x  y<br /></pre></div>
<div class="output"><div class="title">Output</div><pre>YES<br /></pre></div>
</div></div>SECTION</div></div></body></html>"""

ATCODER_PAGE = """<html><head><title>B - サンプル &amp; Test</title></head><body>
<p>Time Limit: 2 sec / Memory Limit: 1024 MiB</p>
<div id="task-statement"><span class="lang"><span class="lang-ja">
<div class="part"><section><h3>入力</h3><pre><var>N</var></pre></section></div>
<div class="part"><section><h3>入力例 1</h3><pre>3
1 2 3
</pre></section></div>
<div class="part"><section><h3>出力例 1</h3><pre>6
</pre></section></div>
<div class="part"><h3>入力例 2</h3>
<section><pre class="prettyprint linenums">
1
5
</pre></section></div>
</span><span class="lang-en">
<h3>Input</h3><pre><var>N</var></pre>
<h3>Sample Input 1</h3><pre>3
1 2 3
</pre>
<h3>Sample Output 1</h3><pre>6
</pre>
</span></span></div></body></html>"""


def leetcode_reply(meta, testcases, content, snippet, translated=None, paid=False):
    question = {"questionFrontendId": "7", "title": "Demo", "translatedTitle": "演示",
                "isPaidOnly": paid, "content": content, "translatedContent": translated,
                "exampleTestcases": testcases, "metaData": json.dumps(meta),
                "codeSnippets": [{"langSlug": "cpp", "code": snippet}]}
    return json.dumps({"data": {"question": question}}).encode()


def example_pre(*pairs):
    return "".join(f"<p><strong class=\"example\">Example {i}:</strong></p><pre>\n<strong>Input:</strong> "
                   f"x\n<strong>Output:</strong> {output}\n<strong>Explanation:</strong> why\n</pre>"
                   for i, output in enumerate(pairs, 1))


TWO_SUM = leetcode_reply(
    {"name": "twoSum", "params": [{"name": "nums", "type": "integer[]"}, {"name": "target", "type": "integer"}],
     "return": {"type": "integer[]"}},
    "[2,7,11,15]\n9\n[3,2,4]\n6\n[3,3]\n6",
    example_pre("[0,1]", "[1,2]", "[0,1]"),
    "class Solution {\npublic:\n    vector<int> twoSum(vector<int>& nums, int target) {\n        \n    }\n};")

LRU = leetcode_reply(
    {"classname": "LRUCache", "constructor": {"params": [{"type": "integer", "name": "capacity"}]},
     "methods": [{"name": "get", "params": [{"type": "integer", "name": "key"}], "return": {"type": "integer"}},
                 {"name": "put", "params": [{"type": "integer", "name": "key"}, {"type": "integer", "name": "value"}],
                  "return": {"type": "void"}}],
     "systemdesign": True},
    '["LRUCache","put","put","get","put","get","put","get","get","get"]\n'
    "[[2],[1,1],[2,2],[1],[3,3],[2],[4,4],[1],[3],[4]]",
    "<pre><strong>Input</strong>\n[...]\n\n<strong>Output</strong>\n"
    "[null, null, null, 1, null, -1, null, -1, 3, 4]\n\n<strong>Explanation</strong>\n...</pre>",
    "class LRUCache {\npublic:\n    LRUCache(int capacity) {\n        \n    }\n    \n"
    "    int get(int key) {\n        \n    }\n    \n    void put(int key, int value) {\n        \n    }\n};")

MIXED = leetcode_reply(
    {"name": "mix", "params": [{"name": "root", "type": "TreeNode"}, {"name": "head", "type": "ListNode"},
                               {"name": "grid", "type": "character[][]"}, {"name": "s", "type": "string"},
                               {"name": "x", "type": "double"}, {"name": "flag", "type": "boolean"},
                               {"name": "big", "type": "long[]"}],
     "return": {"type": "list<string>"}},
    '[1,null,2,3]\n[4,5]\n[["a","b"],["c","d"]]\n"q \\"x\\""\n2.5\ntrue\n[9000000000]',
    "",
    "class Solution {\npublic:\n    vector<string> mix(TreeNode* root, ListNode* head, vector<vector<char>>& grid,"
    " string s, double x, bool flag, vector<long long>& big) {\n        \n    }\n};",
    translated="<div class=\"example-block\"><p><strong>输出：</strong><span class=\"example-io\">"
               "[\"1,2,3\", \"9\", \"ad\", \"q \\\"x\\\"!\", \"5.000\", \"no\", \"9000000001\"]</span></p></div>")

TREE = leetcode_reply(
    {"name": "invertTree", "params": [{"name": "root", "type": "TreeNode"}], "return": {"type": "TreeNode"}},
    "[4,2,7,1,3,6,9]\n[1,null,2,null,3]\n[]", example_pre("[4,7,2,9,6,3,1]", "[1,2,null,3]", "[]"),
    "class Solution {\npublic:\n    TreeNode* invertTree(TreeNode* root) {\n        \n    }\n};")

ROTATE = leetcode_reply(
    {"name": "rotate", "params": [{"name": "matrix", "type": "integer[][]"}], "return": {"type": "void"},
     "output": {"paramindex": 0}},
    "[[1,2],[3,4]]", example_pre("[[3,1],[4,2]]"),
    "class Solution {\npublic:\n    void rotate(vector<vector<int>>& matrix) {\n        \n    }\n};")

REMOVE = leetcode_reply(
    {"name": "removeElement", "params": [{"name": "nums", "type": "integer[]"}, {"name": "val", "type": "integer"}],
     "return": {"type": "integer"}, "output": {"paramindex": 0, "size": "ret"}},
    "[3,2,2,3]\n3", example_pre("2, nums = [2,2,_,_]"),
    "class Solution {\npublic:\n    int removeElement(vector<int>& nums, int val) {\n        \n    }\n};")

SOLUTIONS = {
    "twoSum": "        for (int i = 0; i < (int)nums.size(); ++i)\n"
              "            for (int j = i + 1; j < (int)nums.size(); ++j)\n"
              "                if (nums[i] + nums[j] == target) return {i, j};\n        return {};",
    "LRUCache": "        cap = capacity;",
    "get": "        for (auto it = items.begin(); it != items.end(); ++it)\n"
           "            if (it->first == key) { items.splice(items.begin(), items, it); return it->second; }\n"
           "        return -1;",
    "put": "        if (get(key) != -1) { items.front().second = value; return; }\n"
           "        items.emplace_front(key, value);\n"
           "        if ((int)items.size() > cap) items.pop_back();",
    "mix": "        string tree;\n"
           "        for (TreeNode* t : {root, root->right, root->right->left}) tree += (tree.empty() ? \"\" : \",\") + to_string(t->val);\n"
           "        return {tree, to_string(head->val + head->next->val), string{grid[0][0], grid[1][1]},\n"
           "                s + \"!\", to_string(x * 2).substr(0, 5), flag ? \"no\" : \"yes\", to_string(big[0] + 1)};",
    "invertTree": "        if (!root) return root;\n        swap(root->left, root->right);\n"
                  "        invertTree(root->left);\n        invertTree(root->right);\n        return root;",
    "rotate": "        matrix = {{matrix[1][0], matrix[0][0]}, {matrix[1][1], matrix[0][1]}};",
    "removeElement": "        int k = 0;\n        for (int x : nums) if (x != val) nums[k++] = x;\n        return k;",
}


class TargetTests(unittest.TestCase):
    def test_urls_and_shorthands(self):
        cases = {
            "https://codeforces.com/contest/1610/problem/F": ("codeforces", "cf/1610/F"),
            "https://codeforces.com/problemset/problem/4/a?locale=ru": ("codeforces", "cf/4/A"),
            "https://codeforces.com/gym/104114/problem/B": ("codeforces", "cf/gym/104114/B"),
            "1610F": ("codeforces", "cf/1610/F"),
            "cf/1610/F1": ("codeforces", "cf/1610/F1"),
            "https://atcoder.jp/contests/abc400/tasks/abc400_e": ("atcoder", "atcoder/abc400/E"),
            "abc400_e": ("atcoder", "atcoder/abc400/E"),
            "atcoder/abc400/E": ("atcoder", "atcoder/abc400/E"),
            "tessoku_book_a": ("atcoder", "atcoder/tessoku-book/A"),
            "https://leetcode.com/problems/two-sum/description/": ("leetcode", "leetcode/two-sum"),
            "https://leetcode.cn/contest/weekly-contest-400/problems/two-sum/": ("leetcode", "leetcode/two-sum"),
            "leetcode/two-sum": ("leetcode", "leetcode/two-sum"),
        }
        for spec, (platform, name) in cases.items():
            with self.subTest(spec=spec):
                target = fp.parse_target(spec)
                self.assertEqual((target.platform, target.id), (platform, name))
                cp.problem_name(target.id)  # Every default name is a valid archive name.
        self.assertEqual(fp.parse_target("abc400_e").url, "https://atcoder.jp/contests/abc400/tasks/abc400_e")
        self.assertEqual(fp.parse_target("leetcode/two-sum").url, "https://leetcode.cn/problems/two-sum/")
        self.assertTrue(fp.parse_target("https://leetcode.com/problems/two-sum").url.startswith("https://leetcode.com/"))

    def test_rejects_unknown_input(self):
        for spec in ("two-sum", "https://example.com/problem/1", "https://codeforces.com/blog/entry/1",
                     "https://atcoder.jp/contests/abc400"):
            with self.subTest(spec=spec), self.assertRaises(ValueError):
                fp.parse_target(spec)


class ParserTests(unittest.TestCase):
    def test_codeforces_line_divs_br_and_plain_pre(self):
        problem = fp.parse_codeforces(CODEFORCES_PAGE.replace("SECTION", ""), fp.parse_target("1A"))
        self.assertEqual(problem.title, "F. Demo & Test")
        self.assertEqual(problem.time_limit, "2 seconds")
        self.assertEqual([(s.input, s.output) for s in problem.samples],
                         [("2\n1 2\n", "3\n"), ("8\nx  y\n", "YES\n")])
        self.assertEqual(problem.notes, [])

    def test_codeforces_interactive_and_missing_samples(self):
        for section in ('<div class="section-title">Interaction</div>',
                        "<p><b>This is an interactive problem.</b></p>"):
            page = CODEFORCES_PAGE.replace("SECTION", section)
            self.assertIn("Interactive", fp.parse_codeforces(page, fp.parse_target("1A")).notes[0])
        with self.assertRaisesRegex(ValueError, "No samples"):
            fp.parse_codeforces('<div class="problemindexholder"><div class="problem-statement"></div></div>',
                                fp.parse_target("1A"))
        for page in ("<html>contest page</html>", '<div class="problem-statement">blog post</div>'):
            with self.assertRaisesRegex(ValueError, "No problem statement"):
                fp.parse_codeforces(page, fp.parse_target("1A"))
        with self.assertRaisesRegex(ValueError, "Cloudflare"):
            fp.parse_codeforces("<title>Just a moment...</title>", fp.parse_target("1A"))

    def test_atcoder_deduplicates_languages_and_old_pre_newline(self):
        problem = fp.parse_atcoder(ATCODER_PAGE, fp.parse_target("abc400_b"))
        self.assertEqual(problem.title, "B - サンプル & Test")
        self.assertEqual(problem.time_limit, "2 sec")
        self.assertEqual([(s.input, s.output) for s in problem.samples],
                         [("3\n1 2 3\n", "6\n"), ("1\n5\n", None)])
        self.assertIn("2 sample inputs but 1 outputs", problem.notes[0])
        with self.assertRaisesRegex(ValueError, "No samples"):
            fp.parse_atcoder("<title>Sign In</title>", fp.parse_target("abc400_b"))

    def test_clean_sample(self):
        self.assertEqual(fp.clean_sample("\r\n1 2\r\n\r\n  \n"), "1 2\n")
        self.assertEqual(fp.clean_sample("\n\n3\n"), "\n3\n")  # Only the HTML newline is dropped.
        self.assertEqual(fp.clean_sample("  a\xa0b  \n"), "  a b  \n")
        self.assertEqual(fp.clean_sample("\n"), "")

    def test_leetcode_samples_titles_and_outputs(self):
        problem = fp.parse_leetcode(TWO_SUM, fp.parse_target("https://leetcode.cn/problems/two-sum/"))
        self.assertEqual(problem.title, "7. 演示")
        self.assertEqual([(s.input, s.output) for s in problem.samples],
                         [("[2,7,11,15]\n9\n", "[0,1]\n"), ("[3,2,4]\n6\n", "[1,2]\n"), ("[3,3]\n6\n", "[0,1]\n")])
        self.assertIn("return lc::run_solution(&Solution::twoSum);", problem.code)
        self.assertEqual(fp.parse_leetcode(TWO_SUM, fp.parse_target("https://leetcode.com/problems/two-sum/")).title,
                         "7. Demo")
        design = fp.parse_leetcode(LRU, fp.parse_target("leetcode/lru-cache"))
        self.assertEqual(design.samples[0].output, "[null,null,null,1,null,-1,null,-1,3,4]\n")
        self.assertIn("lc::constructor<LRUCache, int>()", design.code)
        self.assertIn('{"put", lc::method(&LRUCache::put)},', design.code)
        mixed = fp.parse_leetcode(MIXED, fp.parse_target("leetcode/mix"))
        self.assertEqual(mixed.samples[0].output, '["1,2,3","9","ad","q \\"x\\"!","5.000","no","9000000001"]\n')
        removed = fp.parse_leetcode(REMOVE, fp.parse_target("leetcode/remove-element"))
        self.assertIn('lc::run_solution<0, true>(&Solution::removeElement, "nums")', removed.code)
        self.assertIn("first k elements", removed.notes[0])
        self.assertIn("lc::run_solution<0>(&Solution::rotate)",
                      fp.parse_leetcode(ROTATE, fp.parse_target("leetcode/rotate")).code)

    def test_leetcode_errors_and_unsupported_types(self):
        target = fp.parse_target("leetcode/x")
        with self.assertRaisesRegex(ValueError, "not found"):
            fp.parse_leetcode(b'{"data": {"question": null}}', target)
        with self.assertRaisesRegex(ValueError, "Premium"):
            fp.parse_leetcode(leetcode_reply({}, "", None, ""), target)
        with self.assertRaisesRegex(ValueError, "Unexpected reply"):
            fp.parse_leetcode(b"<html>blocked</html>", target)
        graph = leetcode_reply({"name": "cloneGraph", "params": [{"name": "node", "type": "Node"}],
                                "return": {"type": "Node"}}, "[[2],[1]]", example_pre("[[2],[1]]"),
                               "class Solution {\npublic:\n    Node* cloneGraph(Node* node) {\n    }\n};")
        problem = fp.parse_leetcode(graph, target)
        self.assertIn("'Node'", problem.notes[0])
        self.assertIn("return 0;", problem.code)

    def test_compact_array_keeps_strings(self):
        self.assertEqual(fp.compact_array('[ "a b" , [1, 2] ]'), '["a b",[1,2]]')
        self.assertEqual(fp.compact_array('["c\\" d", "e\\\\", " f"]'), '["c\\" d","e\\\\"," f"]')
        self.assertEqual(fp.compact_array("2, nums = [2, 2]"), "2, nums = [2, 2]")
        self.assertEqual(fp.cpp_type("list<list<integer>>"), "vector<vector<int>>")
        self.assertEqual(fp.cpp_type("character[][]"), "vector<vector<char>>")
        self.assertIsNone(fp.cpp_type("Node[]"))


COMPANION = {
    "name": "G.  Castle Defense", "group": "Codeforces - Educational Round 40",
    "url": "https://codeforces.com/problemset/problem/954/G", "interactive": False,
    "memoryLimit": 256, "timeLimit": 1500, "testType": "single",
    "tests": [{"input": "5 0 6\r\n5 4 3 4 9\r\n", "output": "5\n"}, {"input": "4 2 0\n1 2 3 4", "output": "6"}],
    "input": {"type": "stdin"}, "output": {"type": "stdout"}, "batch": {"id": "x", "size": 1},
}


def free_port():
    with socket.socket() as probe:
        probe.bind(("127.0.0.1", 0))
        return probe.getsockname()[1]


def post(port, body, host="127.0.0.1"):
    """POST like the extension does; retries until the listener is up."""
    for _ in range(100):
        try:
            connection = http.client.HTTPConnection(host, port, timeout=5)
            connection.request("POST", "/", body=body, headers={"Content-Type": "application/json"})
            status = connection.getresponse().status
            connection.close()
            return status
        except OSError:
            time.sleep(0.05)
    raise AssertionError("listener never came up")


class CompanionTests(unittest.TestCase):
    def test_payload_becomes_a_problem(self):
        problem = fp.companion_problem(COMPANION)
        self.assertEqual((problem.target.platform, problem.target.id), ("codeforces", "cf/954/G"))
        self.assertEqual(problem.target.url, fp.parse_target("954G").url)  # Same page as 'new 954G'.
        self.assertEqual((problem.title, problem.time_limit), ("G. Castle Defense", "1.5 seconds"))
        self.assertEqual([(s.input, s.output) for s in problem.samples],
                         [("5 0 6\n5 4 3 4 9\n", "5\n"), ("4 2 0\n1 2 3 4\n", "6\n")])
        self.assertEqual((problem.notes, problem.code), ([], None))

    def test_other_sites_notes_and_bad_payloads(self):
        other = fp.companion_problem({
            "name": "P1001 A+B", "url": "https://www.luogu.com.cn/problem/P1001", "timeLimit": 1000,
            "interactive": True, "tests": [{"input": "1 2\n"}, "junk"],
            "input": {"type": "file", "fileName": "a.in"}, "output": {"type": "stdout"}})
        self.assertEqual((other.target.platform, other.target.id), ("www.luogu.com.cn", ""))
        self.assertEqual(other.time_limit, "1 second")
        self.assertEqual([(s.input, s.output) for s in other.samples], [("1 2\n", None)])
        self.assertIn("Interactive", other.notes[0])
        self.assertIn("a.in for input", other.notes[1])
        with self.assertRaisesRegex(ValueError, "no samples"):
            fp.companion_problem({**COMPANION, "tests": []})
        for payload in ({}, {"url": 1, "tests": []}, {"url": "https://codeforces.com/", "tests": None}):
            with self.subTest(payload=payload), self.assertRaisesRegex(ValueError, "not a Competitive Companion"):
                fp.companion_problem(payload)

    def test_listener_receives_posts_and_rejects_junk(self):
        port = free_port()
        statuses = []
        bodies = (b"not json", b"[1, 2]", json.dumps(COMPANION).encode(),
                  json.dumps({**COMPANION, "name": "second"}).encode())
        sender = threading.Thread(target=lambda: statuses.extend(post(port, body) for body in bodies))
        sender.start()
        self.addCleanup(sender.join)
        payloads = fp.companion_payloads(port, wait=20)
        self.assertEqual(next(payloads), COMPANION)
        self.assertEqual(next(payloads)["name"], "second")
        sender.join()
        self.assertEqual(statuses, [400, 400, 200, 200])
        payloads.close()
        with socket.socket() as probe:  # The port is released.
            probe.bind(("127.0.0.1", port))

    def test_listener_answers_within_the_extension_time_limit_despite_idle_connections(self):
        # The extension aborts after 500 ms and says nothing, so a connection that sends no
        # data (browsers open spares) must not hold up the request behind it.
        port = free_port()
        result = {}

        def extension():
            idle = []
            for family, host in ((socket.AF_INET, "127.0.0.1"), (socket.AF_INET6, "::1")):
                for _ in range(100):
                    try:
                        spare = socket.socket(family)
                        spare.connect((host, port))
                        idle.append(spare)
                        break
                    except OSError:
                        spare.close()
                        if family == socket.AF_INET6:
                            break  # No IPv6 loopback here.
                        time.sleep(0.05)
            started = time.monotonic()
            result["status"] = post(port, json.dumps(COMPANION).encode())
            result["seconds"] = time.monotonic() - started
            for spare in idle:
                spare.close()

        sender = threading.Thread(target=extension)
        sender.start()
        self.addCleanup(sender.join)
        payloads = fp.companion_payloads(port, wait=20)
        self.assertEqual(next(payloads), COMPANION)
        sender.join()
        payloads.close()
        self.assertEqual(result["status"], 200)
        self.assertLess(result["seconds"], 0.4)

    def test_listener_times_out_and_reports_a_busy_port(self):
        port = free_port()
        with self.assertRaisesRegex(ValueError, f"Nothing arrived.*Nothing connected to port {port}"):
            next(fp.companion_payloads(port, wait=0.2))

        def visit():  # Someone checking the port in the address bar.
            for _ in range(100):
                try:
                    connection = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
                    connection.request("GET", "/")
                    reply = connection.getresponse()
                    visited.append((reply.status, reply.read().decode()))
                    connection.close()
                    return
                except OSError:
                    time.sleep(0.05)

        visited = []
        visitor = threading.Thread(target=visit)
        visitor.start()
        self.addCleanup(visitor.join)
        with self.assertRaisesRegex(ValueError, "Nothing arrived.*1 connection.*page view"):
            next(fp.companion_payloads(port, wait=1.5))
        visitor.join()
        self.assertEqual(visited[0][0], 200)
        self.assertIn("waiting here for Competitive Companion", visited[0][1])
        with socket.socket() as busy:
            busy.bind(("127.0.0.1", port))
            busy.listen()
            with self.assertRaisesRegex(ValueError, f"Cannot listen on port {port}.*--port"):
                next(fp.companion_payloads(port, wait=0.2))

    def test_cloudflare_challenge_is_told_apart_from_other_errors(self):
        def reply(code, headers, body=b""):
            message = email.message.Message()
            for key, value in headers.items():
                message[key] = value
            return urllib.error.HTTPError("https://codeforces.com/x", code, "Forbidden", message, io.BytesIO(body))

        cases = ((reply(403, {"cf-mitigated": "challenge"}), True),
                 (reply(403, {}, b"<html><head><title>Just a moment...</title>"), True),
                 (reply(403, {}, b"<html>Forbidden</html>"), False),
                 (reply(404, {"cf-mitigated": "challenge"}), False))
        for error, challenged in cases:
            with self.subTest(code=error.code, challenged=challenged), \
                    patch("urllib.request.urlopen", side_effect=error), \
                    self.assertRaises(ValueError) as caught:
                fp.http_request("https://codeforces.com/x")
            self.assertEqual(isinstance(caught.exception, fp.BrowserCheck), challenged)
        self.assertIn("codeforces.com answered with a browser check", str(fp.BrowserCheck("https://codeforces.com/x")))


@unittest.skipUnless(shutil.which(os.environ.get("CP_LEETCODE_CXX", "g++")), "C++ compiler not on PATH")
class LeetCodeDriverTests(unittest.TestCase):
    """Generate main.cpp like 'new', fill in a solution, compile and check every example."""

    @classmethod
    def setUpClass(cls):
        checks = ROOT / "build/checks"
        checks.mkdir(parents=True, exist_ok=True)
        cls.temporary = tempfile.TemporaryDirectory(prefix="leetcode 驱动-", dir=checks)
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.folder = Path(cls.temporary.name)
        cls.args = Namespace(cxx=os.environ.get("CP_LEETCODE_CXX", "g++"), std="c++20", include=[])

    def build(self, name, reply):
        problem = fp.parse_leetcode(reply, fp.parse_target(f"leetcode/{name}"))
        code = problem.code
        for method, body in SOLUTIONS.items():
            stub = f" {method}("
            start = code.find(stub)
            if start >= 0:
                empty = code.index("{\n", start) + 2
                end = code.index("\n    }", empty)
                code = code[:empty] + body + code[end:]
        code = code.replace("class LRUCache {\npublic:", "class LRUCache {\n    int cap = 0;\n"
                            "    list<pair<int, int>> items;\npublic:")
        source = self.folder / f"{name}.cpp"
        source.write_text(code, encoding="utf-8")
        executable = self.folder / name / "program.exe"
        with redirect_stdout(io.StringIO()):
            cp.compile_source(source, executable, self.args)
        return problem, executable

    def test_examples_match_leetcode_output(self):
        for name, reply in (("two-sum", TWO_SUM), ("lru-cache", LRU), ("mix", MIXED), ("invert", TREE),
                            ("rotate", ROTATE), ("remove-element", REMOVE)):
            with self.subTest(problem=name):
                problem, executable = self.build(name, reply)
                for sample in problem.samples:
                    result = cp.execute([str(executable)], sample.input.encode(), 10)
                    self.assertEqual(result.returncode, 0, result.stderr.decode(errors="replace"))
                    self.assertEqual(result.stdout.decode().split(), sample.output.split())
                everything = "".join(sample.input + "\n" for sample in problem.samples).encode()
                together = cp.execute([str(executable)], everything, 10)
                self.assertEqual(together.stdout.decode().split(),
                                 "".join(sample.output for sample in problem.samples).split())

    def test_bad_input_is_reported_with_line_number(self):
        _, executable = self.build("two-sum-errors", TWO_SUM)
        for data, message in ((b"[1,2]\n", b"incomplete test case"), (b"\xef\xbb\xbf[1,x]\n3\n", b"line 1"),
                              (b"[1]\n99999999999\n", b"does not fit"), (b'[1]\n"3"\n', b"expected an integer")):
            with self.subTest(data=data):
                result = cp.execute([str(executable)], data, 10)
                self.assertEqual(result.returncode, 1)
                self.assertIn(message, result.stderr)
        _, design = self.build("lru-errors", LRU)
        result = cp.execute([str(design)], b'["LRUCache","nope"]\n[[1],[]]\n', 10)
        self.assertEqual(result.returncode, 1)
        self.assertIn(b'unknown operation "nope"', result.stderr)


if __name__ == "__main__":
    unittest.main()
