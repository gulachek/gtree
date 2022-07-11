const { Cpp } = require('gulpachek/cpp');
const { version } = require('./package.json');

if (!version) {
	console.error(new Error('gtree package.json version not specified'));
	process.exit(1);
}

function build(sys) {
	const cpp = new Cpp(sys);

	const boost = {
		fiber: cpp.require('org.boost.fiber', '1.78.0')
	};

	const lib = cpp.library('com.gulachek.gtree', version,
		'src/base128.cpp',
		'src/twos_complement.cpp',
		'src/fd.cpp',
		'src/cause.cpp',
		'src/translate.cpp',
		'src/tree.cpp'
	);

	lib.include('include');
	lib.link(boost.fiber);

	return lib;
}

module.exports = {
	build
};
