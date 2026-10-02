"""Fetch problem samples from Codeforces, AtCoder and LeetCode (standard library only)."""
from __future__ import annotations

from dataclasses import dataclass, field
from html.parser import HTMLParser
import json
import re
import time
import urllib.error
import urllib.parse
import urllib.request

USER_AGENT = ("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
              "(KHTML, like Gecko) Chrome/130.0 Safari/537.36")
LEETCODE_DEFAULT_HOST = "leetcode.cn"
VOID_TAGS = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta",
             "source", "track", "wbr"}


@dataclass
class Sample:
    input: str
    output: str | None


@dataclass
class Target:
    platform: str  # codeforces, atcoder or leetcode
    id: str        # Archive name, e.g. cf/1610/F
    url: str       # Canonical problem page
    fetch_url: str = ""


@dataclass
class Problem:
    target: Target
    title: str = ""
    time_limit: str = ""
    samples: list[Sample] = field(default_factory=list)
    notes: list[str] = field(default_factory=list)
    code: str | None = None  # Generated LeetCode main.cpp

    def metadata(self) -> dict:
        return {"id": self.target.id, "platform": self.target.platform, "url": self.target.url,
                "title": self.title, "time_limit": self.time_limit}


# ---------------------------------------------------------------- problem names

def parse_target(spec: str) -> Target:
    """Accept a problem URL, or shorthand such as 1610F, abc400_e, leetcode/two-sum."""
    text = spec.strip()
    if re.match(r"https?://", text, re.I):
        return _target_from_url(text)
    if match := re.fullmatch(r"(?:cf/)?gym/(\d+)/([A-Za-z]\d*)", text, re.I):
        return _codeforces(match[1], match[2], gym=True)
    if match := re.fullmatch(r"(?:cf)?/?(\d+)\s*/?\s*([A-Za-z]\d?)", text, re.I):
        return _codeforces(match[1], match[2])
    if match := re.fullmatch(r"(?:leetcode|lc)/([a-z0-9-]+)", text, re.I):
        return _leetcode(LEETCODE_DEFAULT_HOST, match[1].lower())
    if match := re.fullmatch(r"(?:atcoder/)?([a-z0-9-]+)/([a-z0-9]+)", text, re.I):
        contest = match[1].lower()
        return _atcoder(contest, f"{contest.replace('-', '_')}_{match[2].lower()}")
    if match := re.fullmatch(r"([a-z0-9_]+)_([a-z0-9]+)", text, re.I):
        return _atcoder(match[1].lower().replace("_", "-"), text.lower())
    raise ValueError(f"Unrecognized problem {spec!r}. Paste the problem URL, or use shorthand "
                     "such as 1610F, cf/1610/F, abc400_e, atcoder/abc400/E or leetcode/two-sum.")


def _target_from_url(url: str) -> Target:
    parsed = urllib.parse.urlsplit(url)
    host = (parsed.hostname or "").lower()
    parts = [urllib.parse.unquote(part) for part in parsed.path.split("/") if part]
    if host == "codeforces.com" or host.endswith(".codeforces.com"):
        if len(parts) >= 4 and parts[0] in {"contest", "gym"} and parts[2] == "problem":
            return _codeforces(parts[1], parts[3], gym=parts[0] == "gym")
        if len(parts) >= 4 and parts[:2] == ["problemset", "problem"]:
            return _codeforces(parts[2], parts[3])
        raise ValueError("Use a Codeforces problem URL such as https://codeforces.com/contest/1610/problem/F")
    if host in {"atcoder.jp", "www.atcoder.jp"}:
        if len(parts) >= 4 and parts[0] == "contests" and parts[2] == "tasks":
            return _atcoder(parts[1], parts[3])
        raise ValueError("Use an AtCoder task URL such as https://atcoder.jp/contests/abc400/tasks/abc400_e")
    if host.removeprefix("www.") in {"leetcode.com", "leetcode.cn"}:
        if "problems" in parts and parts.index("problems") + 1 < len(parts):
            return _leetcode(host.removeprefix("www."), parts[parts.index("problems") + 1])
        raise ValueError("Use a LeetCode problem URL such as https://leetcode.cn/problems/two-sum/")
    raise ValueError(f"Unsupported site: {host or url}. Supported: Codeforces, AtCoder, LeetCode.")


def _codeforces(contest: str, index: str, gym: bool = False) -> Target:
    if not contest.isdigit():
        raise ValueError(f"Invalid Codeforces contest id: {contest}")
    index = index.upper()
    gym = gym or int(contest) >= 100000
    kind = "gym" if gym else "contest"
    url = f"https://codeforces.com/{kind}/{contest}/problem/{index}"
    name = f"cf/gym/{contest}/{index}" if gym else f"cf/{contest}/{index}"
    return Target("codeforces", name, url, url + "?locale=en")


def _atcoder(contest: str, task: str) -> Target:
    url = f"https://atcoder.jp/contests/{contest}/tasks/{task}"
    index = task.rsplit("_", 1)[-1].upper()
    return Target("atcoder", f"atcoder/{contest}/{index}", url, url + "?lang=en")


def _leetcode(host: str, slug: str) -> Target:
    slug = slug.lower()
    if not re.fullmatch(r"[a-z0-9][a-z0-9-]*", slug):
        raise ValueError(f"Invalid LeetCode problem slug: {slug}")
    url = f"https://{host}/problems/{slug}/"
    return Target("leetcode", f"leetcode/{slug}", url, f"https://{host}/graphql/")


# ---------------------------------------------------------------- HTTP and HTML

def http_request(url: str, data: bytes | None = None, headers: dict | None = None,
                 timeout: float = 20, attempts: int = 2) -> bytes:
    request = urllib.request.Request(url, data=data, headers={
        "User-Agent": USER_AGENT, "Accept-Language": "en-US,en;q=0.9", **(headers or {})})
    for attempt in range(attempts):
        try:
            with urllib.request.urlopen(request, timeout=timeout) as response:
                return response.read()
        except urllib.error.HTTPError as error:
            if error.code >= 500 and attempt + 1 < attempts:
                time.sleep(1)
                continue
            hint = {403: " (blocked; try again later or open the page in a browser first)",
                    404: " (not found; check the problem id, or the contest may require login)",
                    429: " (rate limited; wait a little and retry)"}.get(error.code, "")
            raise ValueError(f"HTTP {error.code} for {url}{hint}") from None
        except (urllib.error.URLError, TimeoutError, ConnectionError) as error:
            if attempt + 1 < attempts:
                time.sleep(1)
                continue
            reason = getattr(error, "reason", error)
            raise ValueError(f"Cannot reach {url}: {reason}. Check the network or proxy "
                             "settings, or paste the samples into data/ by hand.") from None
    raise AssertionError("unreachable")


class Node:
    __slots__ = ("tag", "attrs", "children", "parent")

    def __init__(self, tag: str, attrs: dict, parent: Node | None):
        self.tag, self.attrs, self.parent = tag, attrs, parent
        self.children: list[Node | str] = []

    def classes(self) -> set[str]:
        return set((self.attrs.get("class") or "").split())

    def iter(self):
        yield self
        for child in self.children:
            if isinstance(child, Node):
                yield from child.iter()

    def find_all(self, tag: str, cls: str | None = None) -> list[Node]:
        return [node for node in self.iter()
                if node.tag == tag and (cls is None or cls in node.classes())]

    def find(self, tag: str, cls: str | None = None) -> Node | None:
        return next(iter(self.find_all(tag, cls)), None)

    def text(self) -> str:
        """Text with <br> and block elements (such as Codeforces line divs) as newlines."""
        parts: list[str] = []

        def walk(node: Node) -> None:
            for child in node.children:
                if isinstance(child, str):
                    parts.append(child)
                elif child.tag == "br":
                    parts.append("\n")
                else:
                    walk(child)
                    if child.tag in {"div", "p", "li", "pre"} and parts and not parts[-1].endswith("\n"):
                        parts.append("\n")
        walk(self)
        return "".join(parts)


class _TreeBuilder(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.root = self.current = Node("#document", {}, None)

    def handle_starttag(self, tag, attrs):
        node = Node(tag, dict(attrs), self.current)
        self.current.children.append(node)
        if tag not in VOID_TAGS:
            self.current = node

    def handle_startendtag(self, tag, attrs):
        self.current.children.append(Node(tag, dict(attrs), self.current))

    def handle_endtag(self, tag):
        node = self.current
        while node.parent is not None and node.tag != tag:
            node = node.parent
        if node.parent is not None:  # Ignore stray end tags.
            self.current = node.parent

    def handle_data(self, data):
        self.current.children.append(data)


def parse_html(page: str) -> Node:
    builder = _TreeBuilder()
    builder.feed(page)
    builder.close()
    return builder.root


def clean_sample(text: str) -> str:
    """Normalize newlines; drop the newline right after <pre> and trailing blank lines."""
    text = text.replace("\r\n", "\n").replace("\r", "\n").replace("\xa0", " ")
    if text.startswith("\n"):
        text = text[1:]
    lines = text.split("\n")
    while lines and not lines[-1].strip():
        lines.pop()
    return "\n".join(lines) + "\n" if lines else ""


def pair_samples(inputs: list[str], outputs: list[str], notes: list[str]) -> list[Sample]:
    if len(inputs) != len(outputs):
        notes.append(f"Found {len(inputs)} sample inputs but {len(outputs)} outputs; check data/samples.")
    return [Sample(text, outputs[i] if i < len(outputs) else None) for i, text in enumerate(inputs)]


# ---------------------------------------------------------------- Codeforces

def parse_codeforces(page: str, target: Target) -> Problem:
    # Missing problems redirect to the home page, whose posts may reuse statement markup.
    holder = parse_html(page).find("div", "problemindexholder")
    statement = holder.find("div", "problem-statement") if holder else None
    if statement is None and re.search(r"<title>\s*Just a moment", page):
        raise ValueError("Codeforces answered with a browser check (Cloudflare). Retry in a "
                         "minute, or copy the samples by hand this time.")
    if statement is None:
        raise ValueError(f"No problem statement at {target.url}. The problem may not exist, "
                         "the contest may not have started, or it may need login.")
    problem = Problem(target)
    header = statement.find("div", "header")
    if header is not None:
        title = header.find("div", "title")
        problem.title = " ".join(title.text().split()) if title else ""
        limit = header.find("div", "time-limit")
        if limit is not None:
            problem.time_limit = limit.text().strip().splitlines()[-1].strip()
    inputs, outputs = [], []
    for block in statement.find_all("div", "sample-test"):
        for child in block.children:
            if not isinstance(child, Node) or (pre := child.find("pre")) is None:
                continue
            if "input" in child.classes():
                inputs.append(clean_sample(pre.text()))
            elif "output" in child.classes():
                outputs.append(clean_sample(pre.text()))
    if not inputs:
        raise ValueError(f"No samples found at {target.url} (PDF-only statements have none).")
    problem.samples = pair_samples(inputs, outputs, problem.notes)
    titles = {" ".join(node.text().split()) for node in statement.find_all("div", "section-title")}
    if (titles & {"Interaction", "Протокол взаимодействия"} or 'title="Interactive problem"' in page
            or "this is an interactive problem" in " ".join(statement.text().split()).lower()):
        problem.notes.append("Interactive problem: samples show the dialogue and cannot be checked by run/test.")
    return problem


# ---------------------------------------------------------------- AtCoder

SAMPLE_HEADING = re.compile(r"(Sample Input|Sample Output|入力例|出力例)\s*(\d+)")


def parse_atcoder(page: str, target: Target) -> Problem:
    root = parse_html(page)
    problem = Problem(target)
    title = root.find("title")
    if title is not None:
        problem.title = " ".join(title.text().split()).removesuffix(" - AtCoder").strip()
    if match := re.search(r"(?:Time Limit|実行時間制限)\s*:\s*([\d.]+\s*sec)", page):
        problem.time_limit = match[1]
    statement = next((node for node in root.iter() if node.attrs.get("id") == "task-statement"), root)
    found: dict[str, dict[int, str]] = {"in": {}, "out": {}}
    pending = None
    for node in statement.iter():
        if node.tag == "h3":
            match = SAMPLE_HEADING.search(" ".join(node.text().split()))
            pending = None
            if match:
                kind = "in" if match[1] in {"Sample Input", "入力例"} else "out"
                pending = (kind, int(match[2]))
        elif node.tag == "pre" and pending is not None:
            # Japanese and English sections repeat the same samples; keep the first copy.
            found[pending[0]].setdefault(pending[1], clean_sample(node.text()))
            pending = None
    if not found["in"]:
        raise ValueError(f"No samples found at {target.url}. The task may not exist or the "
                         "contest may require login.")
    problem.samples = [Sample(found["in"][n], found["out"].get(n)) for n in sorted(found["in"])]
    if len(found["out"]) != len(found["in"]):
        problem.notes.append(f"Found {len(found['in'])} sample inputs but {len(found['out'])} "
                             "outputs; check data/samples.")
    return problem


# ---------------------------------------------------------------- LeetCode

LEETCODE_QUERY = """query questionData($titleSlug: String!) {
  question(titleSlug: $titleSlug) {
    questionFrontendId title translatedTitle isPaidOnly content translatedContent
    exampleTestcases metaData codeSnippets { langSlug code }
  }
}"""
CPP_TYPES = {"integer": "int", "long": "long long", "double": "double", "boolean": "bool",
             "string": "string", "character": "char", "ListNode": "ListNode*",
             "TreeNode": "TreeNode*"}


def cpp_type(name: str) -> str | None:
    """Map LeetCode metadata types (integer[][], list<string>, ...) to the local driver."""
    name = name.strip()
    inner = name[:-2] if name.endswith("[]") else None
    if match := re.fullmatch(r"list<(.+)>", name):
        inner = match[1]
    if inner is not None:
        element = cpp_type(inner)
        return f"vector<{element}>" if element else None
    return CPP_TYPES.get(name)


def compact_array(value: str) -> str:
    """Write [1, 2] as [1,2] like the driver; spaces inside string literals stay."""
    if not value.startswith("["):
        return value
    result, quoted, escaped = [], False, False
    for char in value:
        if escaped:
            escaped = False
        elif quoted:
            escaped = char == "\\"
            quoted = char != '"'
        elif char == '"':
            quoted = True
        elif char.isspace():
            continue
        result.append(char)
    return "".join(result)


def leetcode_outputs(content: str) -> list[str]:
    """Read 'Output: ...' / '输出：...' lines from example blocks."""
    lines = parse_html(content).text().splitlines()
    outputs = []
    for index, line in enumerate(lines):
        if match := re.match(r"\s*(?:Output|输出)\s*[:：]\s*(\S.*)$", line):
            outputs.append(compact_array(match[1].strip()))
        elif re.fullmatch(r"\s*(?:Output|输出)\s*[:：]?\s*", line):  # Design problems: next line.
            following = next((rest.strip() for rest in lines[index + 1:] if rest.strip()), "")
            outputs.append(compact_array(following))
    return outputs


def leetcode_driver(meta: dict) -> tuple[str | None, str | None]:
    """Return (main function body, unsupported type)."""
    def unsupported(types):
        return next((t for t in types if t != "void" and cpp_type(t) is None), None)

    if meta.get("systemdesign") or "classname" in meta:
        name = meta["classname"]
        constructor = [p["type"] for p in meta.get("constructor", {}).get("params", [])]
        methods = meta.get("methods", [])
        types = constructor + [p["type"] for m in methods for p in m.get("params", [])]
        types += [m.get("return", {}).get("type", "void") for m in methods]
        if bad := unsupported(types):
            return None, bad
        arguments = "".join(", " + cpp_type(t) for t in constructor)
        lines = [f"    return lc::run_design(lc::constructor<{name}{arguments}>(), {{"]
        lines += [f'        {{"{m["name"]}", lc::method(&{name}::{m["name"]})}},' for m in methods]
        return "\n".join(lines + ["    });"]), None
    params = meta.get("params", [])
    if bad := unsupported([p["type"] for p in params] + [meta.get("return", {}).get("type", "void")]):
        return None, bad
    output = meta.get("output") or {}
    method = f"&Solution::{meta['name']}"
    index = output.get("paramindex")
    if isinstance(index, int) and 0 <= index < len(params):
        if output.get("size") == "ret":
            return f'    return lc::run_solution<{index}, true>({method}, "{params[index]["name"]}");', None
        return f"    return lc::run_solution<{index}>({method});", None
    return f"    return lc::run_solution({method});", None


def leetcode_main(problem: Problem, snippet: str, meta: dict) -> str:
    body, bad = leetcode_driver(meta)
    if body is None:
        problem.notes.append(f"The local driver does not support LeetCode type {bad!r}; "
                             "main() in main.cpp needs to be written by hand.")
        body = (f"    // Unsupported LeetCode type {bad!r}: read data/input.txt and call the\n"
                "    // solution yourself.\n    return 0;")
    snippet = "\n".join(line.rstrip() for line in snippet.strip("\n").splitlines())
    return (f"// LeetCode {problem.title}\n// {problem.target.url}\n"
            "// Submit only the code between the two markers; the rest runs it locally.\n"
            "// Local input uses LeetCode's test case format: one value per line.\n"
            '#include "leetcode.h"\n\n'
            "// ---- LeetCode submission begins ----\n"
            f"{snippet}\n"
            "// ---- LeetCode submission ends ----\n\n"
            f"int main() {{\n{body}\n}}\n")


def parse_leetcode(body: bytes, target: Target) -> Problem:
    try:
        question = (json.loads(body).get("data") or {}).get("question")
    except (json.JSONDecodeError, AttributeError):
        raise ValueError(f"Unexpected reply from {target.fetch_url}; try again later.") from None
    if not question:
        raise ValueError(f"LeetCode problem not found: {target.url}")
    content = question.get("content") or question.get("translatedContent")
    if not content:
        raise ValueError("LeetCode returned no statement. Premium problems need a logged-in "
                         "session, which this tool does not support.")
    problem = Problem(target)
    chinese = target.url.startswith("https://leetcode.cn/") and question.get("translatedTitle")
    problem.title = f"{question['questionFrontendId']}. {chinese or question['title']}"
    meta = json.loads(question.get("metaData") or "{}")
    design = bool(meta.get("systemdesign") or "classname" in meta)
    per_case = 2 if design else len(meta.get("params", []))
    lines = [line for line in (question.get("exampleTestcases") or "").splitlines() if line.strip()]
    if per_case == 0 or not lines:
        raise ValueError("LeetCode returned no example test cases for this problem.")
    if len(lines) % per_case:
        problem.notes.append(f"Example test cases have {len(lines)} lines, not a multiple of "
                             f"{per_case}; the last example is incomplete.")
    inputs = ["\n".join(lines[i:i + per_case]) + "\n"
              for i in range(0, len(lines) - per_case + 1, per_case)]
    outputs = [value + "\n" for value in leetcode_outputs(content)]
    problem.samples = pair_samples(inputs, outputs, problem.notes)
    if (meta.get("output") or {}).get("size") == "ret":
        problem.notes.append("Only the first k elements are judged, often in any order; "
                             "differences reported by test may still be accepted.")
    snippet = next((s["code"] for s in question.get("codeSnippets") or [] if s["langSlug"] == "cpp"), None)
    if snippet is None:
        problem.notes.append("LeetCode offers no C++ code for this problem; main.cpp was not generated.")
    else:
        problem.code = leetcode_main(problem, snippet, meta)
    return problem


# ---------------------------------------------------------------- entry point

def fetch(target: Target) -> Problem:
    if target.platform == "leetcode":
        slug = target.id.split("/", 1)[1]
        data = json.dumps({"query": LEETCODE_QUERY, "variables": {"titleSlug": slug},
                           "operationName": "questionData"}).encode()
        body = http_request(target.fetch_url, data=data, headers={
            "Content-Type": "application/json", "Referer": target.url})
        return parse_leetcode(body, target)
    page = http_request(target.fetch_url).decode("utf-8", errors="replace")
    if target.platform == "codeforces":
        return parse_codeforces(page, target)
    return parse_atcoder(page, target)
