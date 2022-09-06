const { series, parallel } = require('bach');
const asyncDone = require('async-done');
const { BuildSystem } = require('gulpachek');
const { CppSystem } = require('gulpachek/cpp');
const fs = require('fs');
const { spawn } = require('child_process');
const { version } = require('./package.json');
const { Command } = require('commander');

if (!version) {
	console.error(new Error('gtree package.json version not specified'));
	process.exit(1);
}

function configure() {
	const sys = new BuildSystem();
	const cpp = new CppSystem({
		sys,
		cppVersion: 20
	});

	return { sys, cpp };
}

function makeLib(opts) {
	const { cpp } = opts;

	const boost = {
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

	return gtree;
}

const program = new Command();

program
  .name('build.js')
  .description('Build gtree libraries')
  .option('--build-type <type>', 'debug or release')
  .version(version);

program.command('build')
.description('Build the library')
.action((_, options) => {

	const { cpp, sys } = configure();

	const gtree = makeLib({ cpp });

	sys.build(gtree.image().binary());

});

program.command('test')
.description('Build and run unit tests')
.action((options) => {
	const { cpp, sys } = configure();

	const gtree = makeLib({ cpp });

	const boost = {
		test: cpp.require('org.boost.test', '1.74.0', 'dynamic')
	};

	const image = gtree.image();

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

		test.link(image);
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

	const asyncCb = (err) => {
		if (err) { console.error(err); }
	}

	const buildRule = sys.rule(image.binary());

	asyncDone(series(buildRule, testRule, ...tests), asyncCb);
});

program.command('pack')
.description('Package build outputs for installers')
.action(async (options) => {
	for (const opts of [
		{ buildType: 'debug' },
		{ buildType: 'release' }
	]) {
		process.env.GULPACHEK_BUILD_TYPE = opts.buildType;
		const { cpp, sys } = configure(opts);

		const gtree = makeLib({ cpp });

		await sys.build(cpp.pack(gtree.image()));
	}
});

program.command('test-install')
.description('Build programs against installed library')
.action((options) => {
	const { cpp, sys } = configure();

	const gtree = cpp.require('com.gulachek.gtree', version, 'dynamic');

	const gtree2hex = cpp.compile({
		name: 'gtree2hex',
		version,
		src: ['tools/gtree2hex.cpp']
	});

	gtree2hex.link(gtree);
	sys.build(gtree2hex.executable());

	const nums = cpp.compile({
		name: 'nums',
		src: ['example/nums.cpp']
	});

	nums.link(gtree);
	sys.build(nums.executable());
});

program.parse();
