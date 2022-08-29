const { task, series, parallel, src, dest } = require('gulp');
const { BuildSystem } = require('gulpachek');
const { CppSystem } = require('gulpachek/cpp');
const fs = require('fs');
const { spawn } = require('child_process');
const { version } = require('./package.json');

if (!version) {
	console.error(new Error('gtree package.json version not specified'));
	process.exit(1);
}

const sys = new BuildSystem(__dirname);
const cpp = new CppSystem({
	sys,
	cppVersion: 20
});

const boost = {
	test: cpp.require('org.boost.test', '1.74.0', 'dynamic'),
	fiber: cpp.require('org.boost.fiber', '1.74.0', 'dynamic'),
	iostreams: cpp.require('org.boost.iostreams', '1.74.0', 'dynamic')
};

const gtree = cpp.compile({
	name: 'com.gulachek.gtree',
	version,
	apiDef: 'GTREE_API',
	src: [
		'src/base128.cpp',
		'src/twos_complement.cpp',
		'src/fd.cpp',
		'src/cause.cpp',
		'src/translate.cpp',
		'src/tree.cpp'
	]
});

gtree.include('include');
gtree.link(boost.fiber);
gtree.link(boost.iostreams);

const image = gtree.image();

const buildRules = [];
const tests = [];

// not ideal that this is using relative path. should be relative to srcdir
for (const f of fs.readdirSync('test')) {
	if (!f.match(/_test.cpp$/)) { continue; }
	const name = f.substring(0, f.length - '_test.cpp'.length);

	const test = cpp.compile({
		name: `${name}_test`,
		src: [`test/${f}`]
	});

	test.link(image);
	test.link(boost.test);
	test.include('test/include');

	const target = test.executable();
	buildRules.push(target);

	const runTest = () => {
		return spawn(target.abs(), [], { stdio: 'inherit' });
	};

	Object.defineProperty(runTest, 'name', {
		value: target.path().toString(),
		writable: false
	});

	tests.push(runTest);
}

const gtree2hex = cpp.compile({
	name: 'gtree2hex',
	version,
	src: ['tools/gtree2hex.cpp']
});

gtree2hex.link(image);
buildRules.push(gtree2hex.executable());

const nums = cpp.compile({
	name: 'nums',
	src: ['example/nums.cpp']
});

nums.link(image);
buildRules.push(nums.executable());

task('build', series(...buildRules.map((rule) => sys.rule(rule))));
task('test', series('build', ...tests));
task('default', series('test'));
