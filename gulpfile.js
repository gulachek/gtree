const { task, series, src, dest } = require('gulp');
const { BuildSystem } = require('gulpachek');
const { Cpp } = require('gulpachek/cpp');
const fs = require('fs');
const { spawn } = require('child_process');

const sys = new BuildSystem(__dirname);
const cpp = new Cpp(sys);

const boost = {
	test: cpp.require('org.boost.test', '1.78.0'),
	fiber: cpp.require('org.boost.fiber', '1.78.0')
};

// shouldn't have to specify ".a"
const lib = cpp.library('com.gulachek.gtree', '0.1.0',
	'src/base128.cpp',
	'src/twos_complement.cpp',
	'src/fd.cpp',
	'src/cause.cpp',
	'src/translate.cpp',
	'src/tree.cpp'
);

lib.include('include');
lib.link(boost.fiber);

const buildRules = [];
const tests = [];

// not ideal that this is using relative path. should be relative to srcdir
for (const f of fs.readdirSync('test')) {
	if (!f.match(/_test.cpp$/)) { continue; }
	const name = f.substring(0, f.length - '_test.cpp'.length);

	const test = cpp.executable(`${name}_test`, `test/${f}`);

	test.link(lib);
	test.link(boost.test);
	test.include('test/include');
	buildRules.push(test);

	const runTest = () => {
		return spawn(test.abs(), [], { stdio: 'inherit' });
	};

	Object.defineProperty(runTest, 'name', {
		value: test.path().toString(),
		writable: false
	});

	tests.push(runTest);
}

const gtree2hex = cpp.executable('gtree2hex',
	'tools/gtree2hex.cpp'
);

gtree2hex.link(lib);
buildRules.push(gtree2hex);

const nums = cpp.executable('nums',
	'example/nums.cpp'
);

nums.link(lib);
buildRules.push(nums);

task('build', series(...buildRules.map((rule) => sys.rule(rule))));
task('test', series('build', ...tests));
task('default', series('test'));
task('install', sys.rule(lib.libroot()));
