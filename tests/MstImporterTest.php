<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use StatonLab\TripalTestSuite\Database\Factory;

/**
 * @class
 * Load MSTmap files.
 */
class MstImporterTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Test MSTmapImporter::loadMapMetadata().
   *
   * @dataProvider provideMapMetadata
   */
  public function testLoadMapMetadata($args) {
    $file = ['file_local' => __DIR__ . '/example_files/single_linkage_group_mst.txt'];

    // Run the function.
    module_load_include('inc', 'tripal_map_helper', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($run_args, $file);
    $importer->loadMapMetadata($args);

    // Check the featuremap was created.
    $map = chado_select_record('featuremap', ['featuremap_id'], [
      'name' => $args['featuremap_name'],
      'unittype_id' => ['name' => $args['featuremap_unittype_name']],
    ]);
    $this->assertNotEmpty($map,
      "Unable to find featuremap record with name " . $args['featuremap_name']);

    // Check the analysis was created.
    $analysis = chado_select_record('analysis', ['analysis_id'], [
      'program' => $args['analysis_program'],
      'programversion' => $args['analysis_programversion'],
      'description' => $args['analysis_description'],
    ]);
    $this->assertNotEmpty($analysis,
      "Unable to find analysis for featuremap " . $args['featuremap_name']);

    // And connected to the current featuremap.
    $link = chado_select_record('featuremap_analysis', ['featuremap_analysis_id'], [
      'analysis_id' => $analysis[0]->analysis_id,
      'featuremap_id' => $map[0]->featuremap_id,
    ]);
    $this->assertNotEmpty($link,
      "Unable to connect the featuremap to the analysis.");

    // Check that there is the correct organism connected to this featuremap.
    $org_link = chado_select_record('featuremap_organism', ['featuremap_organism_id'], [
      'featuremap_id' => $map[0]->featuremap_id,
      'organism_id' => $args['organism_organism_id'],
    ]);
    $this->assertNotEmpty($org_link,
      "Unable to connect the featuremap to the correct organism.");

    // Check map_type and other properties
    // (i.e. population_size, population_type, publication_map_name).
    $properties = [
      'map_type', 'population_type', 'population_size', 'published_map_name',
    ];
    foreach ($properties as $type_name) {
      if (isset($args[$type_name]) and !empty($args[$type_name])) {
        $prop = chado_select_record('featuremapprop', ['featuremapprop_id'], [
          'type_id' => ['name' => $type_name],
          'value' => $args[$type_name],
          'featuremap_id' => $map[0]->featuremap_id,
        ]);
        $this->assertNotEmpty($prop,
          "Unable to find property, $type_name, for map, " . $args['featuremap_name']);
      }
    }
  }

  /**
   * Test MSTmapImporter::loadMapFile().
   *
   * @dataProvider provideMapMetadata
   */
  public function testLoadMapFile($args) {
    $file = ['file_local' => __DIR__ . '/example_files/single_linkage_group_mst.txt'];

    // Run the function.
    module_load_include('inc', 'tripal_map_helper', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($run_args, $file);

    $file_path = $file['file_local'];
    $featuremap = factory('chado.featuremap')->create();
    $featuremap_id = $featuremap->featuremap_id;
    $success = $importer->loadMapFile($file_path, $featuremap_id, $args);

    $this->assertNotFalse($success);

    // There should be 1 linkage group (lg0).
    $linkage_group_ids = chado_query("
      SELECT map_feature_id FROM {featurepos} WHERE featuremap_id=:id GROUP BY map_feature_id
    ", [':id' => $featuremap_id])->fetchCol();
    $this->assertEquals(1, count($linkage_group_ids),
      "There was not exactly one linkage group.");

    // And 100 loci/positions.
    $num_loci = chado_query("
      SELECT count(feature_id) FROM {featurepos} WHERE featuremap_id=:id
    ", [':id' => $featuremap_id])->fetchField();
    $this->assertEquals(100, $num_loci,
      "There was not exactly 100 loci/positions.");
  }

  /**
   * Data Provider to test the loading functions.
   */
  public function provideMapMetadata() {
    $faker = \Faker\Factory::create();
    $set = [];

    $organism = factory('chado.organism')->create();

    // Comprehensive (all form elements filled out.
    $set[] = [
      [
        'featuremap_name' => $faker->words(4, TRUE),
        'published_map_name' => $faker->words(5, TRUE),
        'organism_organism_id' => $organism->organism_id,
        'featuremap_unittype_name' => 'cM',
        'map_type' => 'linkage',
        'population_type' => 'F2',
        'population_size' => $faker->randomDigitNotNull(),
        'analysis_program' => $faker->name,
        'analysis_programversion' => $faker->randomFloat(2, 1, 5),
        'analysis_sourcename' => $faker->words(8, TRUE),
        'analysis_description' => $faker->sentences(2, TRUE),
        'featuremap_description' => $faker->paragraphs(5, TRUE),
      ],
    ];

    // Only required.
    $set[] = [
      [
        'featuremap_name' => $faker->words(3, TRUE),
        'organism_organism_id' => $organism->organism_id,
        'analysis_program' => $faker->name,
        'analysis_programversion' => $faker->randomFloat(2, 1, 5),
        'analysis_sourcename' => $faker->words(5, TRUE),
        'map_type' => $faker->name,
        'featuremap_unittype_name' => 'cM',
      ],
    ];

    return $set;
  }

  /**
   * Test the form.
   */
  public function testLoaderForm() {
    $file = ['file_local' => __DIR__ . '/example_files/single_linkage_group_mst.txt'];

    // Run the function.
    module_load_include('inc', 'tripal_map_helper', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($run_args, $file);

    $form = [];
    $form_state = [];
    $form = $importer->form($form, $form_state);

    $this->assertNotEmpty($form,
      "Failed to ensure we have a form.");

    // Check all required fields are present.
    // DB Requirment:  analysis.program, analysis.programversion,
    // organism.organism_id.
    //
    // Tripal Map Required: featuremap.name, featuremapprop.value (map_type),
    // featuremap.unittype_id.
    $required = [
      'organism_organism_id', 'analysis_program', 'analysis_programversion', 'analysis_sourcename',
      'featuremap_name', 'map_type', 'featuremap_unittype_name',
    ];
    foreach ($form as $key => $element) {
      if (isset($element['#required']) and $element['#required']) {
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
    $faker = \Faker\Factory::create();
    $organism = factory('chado.organism')->create();
    $args = [
      'featuremap_name' => $faker->name,
      'organism_organism_id' => $organism->organism_id,
      'analysis_program' => $faker->name,
      'analysis_programversion' => $faker->randomFloat(2, 3, 5),
      'map_type' => $faker->name,
    ];

    // Run the function.
    module_load_include('inc', 'tripal_map_helper', 'includes/TripalImporter/MSTmapImporter');
    $importer = new \MSTmapImporter();
    $importer->create($args, $file);

    // Supress tripal errors.
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");
    ob_start();

    $success = $importer->run();

    // Clean the buffer and unset tripal errors suppression.
    ob_end_clean();
    putenv("TRIPAL_SUPPRESS_ERRORS");

    $this->assertNotFalse($success,
      "The run function should execute without errors.");
  }

}
