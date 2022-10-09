const { series, parallel } = require('bach');
const { CppBuildCommand } = require('gulpachek/cpp');
const fs = require('fs');
const { spawn } = require('child_process');
const { version } = require('./package.json');
const { Command } = require('commander');

if (!version) {
	console.error(new Error('gtree package.json version not specified'));
	process.exit(1);
}

const program = new Command();
const cppBuild = new CppBuildCommand({
	program,
	cppVersion: 20
});

function makeLib(cpp) {
	const boost = {
		fiber: cpp.require('org.boost.fiber', '1.74.0'),
		iostreams: cpp.require('org.boost.iostreams', '1.74.0')
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

	return gtree;
}

cppBuild.build((args) => {
	const { cpp } = args;

	const gtree = makeLib(cpp);

	// ew
	return cpp.toLibrary(gtree).binary();
});

const test = program.command('test')
.description('Build and run unit tests');

cppBuild.configure(test, (args) => {
	const { cpp, sys } = args;

	const gtree = makeLib(cpp);

	const boost = {
		test: cpp.require('org.boost.unit-test-framework', '1.74.0')
	};

	const testBuildRules = [];
	const tests = [];

	// not ideal that this is using relative path. should be relative to srcdir
	for (const f of fs.readdirSync('test')) {
		if (!f.match(/_test.cpp$/)) { continue; }
		const name = f.substring(0, f.length - '_test.cpp'.length);

		const test = cpp.compile({
			name: `${name}_test`,
			src: [`test/${f}`]
		});

		test.link(gtree);
		test.link(boost.test);
		test.include('test/include');

		const target = test.executable();
		testBuildRules.push(target);

		const runTest = () => {
			return spawn(target.abs(), [], { stdio: 'inherit' });
		};

		Object.defineProperty(runTest, 'name', {
			value: target.path().toString(),
			writable: false
		});

		tests.push(runTest);
	}

	const testRule = parallel(...testBuildRules.map(rule => sys.rule(rule)));

	return series(testRule, ...tests);
});

cppBuild.pack((args) => {
	const { cpp } = args;
	return makeLib(cpp);
});

const testInstall = program.command('test-install')
	.description('Build programs against installed library');

cppBuild.configure(testInstall, (args) => {
	const { sys, cpp } = args;

	const gtree = cpp.require('com.gulachek.gtree', version);

	const gtree2hex = cpp.compile({
		name: 'gtree2hex',
		version,
		src: ['tools/gtree2hex.cpp']
	});

	gtree2hex.link(gtree);

	const nums = cpp.compile({
		name: 'nums',
		src: ['example/nums.cpp']
	});

	nums.link(gtree);

	sys.build(gtree2hex.executable());
	sys.build(nums.executable());
});

program.parse();
