#define BOOST_TEST_MODULE test_command_line

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <build_properties.h>

#include "../src/license_generator/command_line-parser.hpp"
#include "../src/ini/SimpleIni.h"
#include "../src/base_lib/base.h"

namespace fs = boost::filesystem;
using namespace license;
using namespace std;

namespace license {
namespace test {
BOOST_AUTO_TEST_CASE(product_initialize_issue_license) {
	const string project_name("TEST");
	const fs::path mock_source_folder(fs::path(PROJECT_TEST_SRC_DIR) / "data" / "src");
	const fs::path projects_folder(fs::path(PROJECT_TEST_TEMP_DIR) / "lcc_projects");
	const fs::path expected_project_folder(projects_folder / project_name);
	const fs::path expectedPrivateKey(projects_folder / project_name / PRIVATE_KEY_FNAME);
	const fs::path expected_public_key(projects_folder / project_name / "include" / "licensecc" / project_name /
									   PUBLIC_KEY_INC_FNAME);

	fs::remove_all(projects_folder);
	BOOST_CHECK_MESSAGE(!fs::exists(expectedPrivateKey),
						"Private key " + expectedPrivateKey.string() + " can't be deleted.");
	BOOST_CHECK_MESSAGE(!fs::exists(expected_public_key),
						"Public key " + expected_public_key.string() + " can't be deleted.");

	int argc = 9;
	const char *argv1[] = {"lcc",
						   "project",
						   "init",
						   "-n",
						   project_name.c_str(),
						   "--projects-folder",
						   projects_folder.string().c_str(),
						   "--templates",
						   mock_source_folder.string().c_str()};
	// initialize_project
	int result = CommandLineParser::parseCommandLine(argc, argv1);
	BOOST_CHECK_EQUAL(result, 0);
	BOOST_REQUIRE_MESSAGE(fs::exists(expectedPrivateKey), "Private key " + expectedPrivateKey.string() + " created.");
	BOOST_CHECK_MESSAGE(fs::exists(expected_public_key), "Public key " + expected_public_key.string() + " created.");

	// issue license in standard location
	argc = 9;
	const char *argv2[] = {"lcc",
						   "license",
						   "issue",
						   "--" PARAM_PRIMARY_KEY,
						   expectedPrivateKey.string().c_str(),
						   "--" PARAM_LICENSE_NAME,
						   "my_license",
						   "--" PARAM_PROJECT_FOLDER,
						   expected_project_folder.string().c_str()};
	result = CommandLineParser::parseCommandLine(argc, argv2);
	BOOST_CHECK_EQUAL(result, 0);
	fs::path expected_license(expected_project_folder / "licenses" / "my_license.lic");
	BOOST_REQUIRE_MESSAGE(fs::exists(expected_license), "License " + expected_license.string() + " created.");
	// load a license, check the project name corresponds and there are no extra elements.
	CSimpleIniA ini;
	ini.LoadFile(expected_license.c_str());
	BOOST_CHECK_MESSAGE(ini.GetSectionSize(project_name.c_str()) == 2, "Section [" + project_name + "] has 2 elements");
}

}  // namespace test
}  // namespace license
