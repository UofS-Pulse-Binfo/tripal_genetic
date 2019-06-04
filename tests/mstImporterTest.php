<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class mstImporterTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Test MSTmapImporter::loadMapMetadata().
   * @dataProvider provideMapMetadata
   */
  public function testLoadMapMetadata($args) {
    $file = ['file_local' => __DIR__ . '/example_files/single_linkage_group_mst.txt'];

    // Run the function.
    module_load_include('inc', 'tripal_genetic', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($run_args, $file);
    $importer->loadMapMetadata($args);

    // Check the featuremap was created.
    $map = chado_select_record('featuremap', ['featuremap_id'], [
      'name' => $args['name'],
      'description' => $args['description'],
    ]);
    $this->assertNotEmpty($map,
      "Unable to find featuremap record with name ".$args['name']);

    // Check the analysis was created.
    $analysis = chado_select_record('analysis', ['analysis_id'], [
      'program' => $args['software_name'],
      'programversion' => $args['software_version'],
      'description' => $args['analysis_description'],
    ]);
    $this->assertNotEmpty($analysis,
      "Unable to find analysis for featuremap ".$args['name']);

    // And connected to the current featuremap.
    // @todo can't yet since featuremap_analysis doesn't exist.

  }

  public function provideMapMetadata() {
    $faker = Factory::create();    
    $set = [];

    // Comprehensive (all form elements filled out.
    $set[] = [[
      'name' => $faker->words(4, TRUE),
      'pub_map_name' => $faker->words(5, TRUE),
      'species_abbrev' => 'Tripalus',
      'units' => 'cM',
      'map_type' => 'linkage',
      'pop_type' => 'F2',
      'pop_size' => $faker->randomDigitNotNull(),
      'contact' => $faker->name,
      'software_name' => $faker->name,
      'software_version' => $faker->randomFloat(2, 1, 5),
      'analysis_description' => $faker->sentences(2, TRUE),
      'description' => $faker->paragraphs(5, TRUE),
    ]];

    // Only required.
    $set[] = [[
      'name' => $faker->words(3, TRUE),
      'species_abbrev' => 'Tripalus',
      'software_name' => $faker->name,
      'software_version' => $faker->randomFloat(2,1,5),
    ]];

    return $set;
  }

  /**
   * Test the form.
   */
  public function testLoaderForm() {
    $file = ['file_local' => __DIR__ . '/example_files/single_linkage_group_mst.txt'];

     // Run the function.
    module_load_include('inc', 'tripal_genetic', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($run_args, $file); 

    $form = [];
    $form_state = [];
    $form = $importer->form($form, $form_state);

    $this->assertNotEmpty($form,
      "Failed to ensure we have a form.");

    // Check all required fields are present.
    $required = ['name','species_abbrev','software_name','software_version'];
    foreach($form as $key => $element) {
      if (isset($element['#required']) AND $element['#required']) {
        $this->assertContains($key, $required,
          "Unexpected form element, $key, marked as required.");
      }
      else {
        $this->assertNotContains($key, $required,
          "Required field, $key, not marked as required.");
      }
    }
  }

  /**
   * Test that run() runs...
   */
  public function testRun() {
    $file = ['file_local' => __DIR__ . '/example_files/single_linkage_group_mst.txt'];
    $args = [
      'name' => 'Lazy Map',
      'species_abbrev' => 'Tripalus',
      'software_name' => 'MSTmap',
      'software_version' => 'unknown',
    ];

     // Run the function.
    module_load_include('inc', 'tripal_genetic', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($args, $file); 

    // Supress tripal errors
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");
    ob_start();

    $success = $importer->run();

    // Clean the buffer and unset tripal errors suppression
    ob_end_clean();
    putenv("TRIPAL_SUPPRESS_ERRORS");

    $this->assertNotFalse($success,
      "The run function should execute without errors.");
  }
}
