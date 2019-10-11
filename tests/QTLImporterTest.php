<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\DatabaseSeeders\GeneticMapSeeder;
use Faker\Factory;

class QTLImporterTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Tests the core importer.
   *
   * @group working
   */
  public function testRun() {

    $seeder = GeneticMapSeeder::seed();
    $mapdetails = $seeder->getDetails();

    $trait_term = factory('chado.cvterm')->create([
      'name' => 'Days to Flowering',
    ]);

    // Instatiate the loader.
    $args = [
      'featuremap_name' => $mapdetails['featuremap_id'],
      'trait_cv_id' => $trait_term->cv_id,
    ];
    $file = 'example_files/qtl.singletrait.tsv';
    $importer = $this->instantiateImporter($file, $args);
    $importer->prepareFiles();

    // Now we run the importer!
    $success = $importer->run();
    $this->assertNotFalse($success,
      "The importer returned an error.");
  }

  /**
   * Tests the helper functions for the QTL importer.
   */
  public function testHelpers() {

    // Instatiate the loader.
    $featuremap = factory('chado.featuremap')->create();
    $args = [
      'featuremap_name' => $featuremap->featuremap_id,
    ];
    $file = 'example_files/qtl.singletrait.tsv';
    $importer = $this->instantiateImporter($file, $args);

    // Values for testing.
    $faker = Factory::create();
    $organism = factory('chado.organism')->create();
    $values = [
      'name' => $faker->name(),
      'uniquename' => $faker->word() . uniqid(),
      'organism_id' => $organism->organism_id,
      'type_id' => ['name' => 'QTL', 'cv_id' => ['name' => 'sequence']],
    ];

    // TEST lookup_id(): no record.
    $retrieved_id = $importer->lookup_id('feature',$values,'feature_id');
    $this->assertFalse($retrieved_id, "Incorrectly found an id that shouldn't exist?");

    // TEST save_datapoint(): insert new.
    $id = $importer->save_datapoint('feature',$values,'feature_id');
    $this->assertNotFalse($id, "Unable to insert the feature using save_datapoint().");
    $this->assertTrue(is_numeric($id), "Feature ID returned from save_datapoint() is not numeric.");

    // TEST lookup_id(): existing record.
    $retrieved_id = $importer->lookup_id('feature',$values,'feature_id');
    $this->assertEquals($id, $retrieved_id, "Unable to lookup the newly created feature with lookup_id().");

    // TEST save_datapoint(): update existing.
    $new_id = $importer->save_datapoint('feature',$values,'feature_id');
    $this->assertNotFalse($new_id, "Unable to update the feature using save_datapoint().");
    $this->assertTrue(is_numeric($new_id), "Feature ID returned from save_datapoint() is not numeric.");
    $this->assertEquals($id, $new_id, "ID returned from save_datapoint() does not match the previously inserted one.");

    // TEST save_feature_property: new property.
    $symbol = $faker->word();
    $success = $importer->save_feature_property($id, 'MAIN', 'published_symbol', $symbol);
    $this->assertNotFalse($success, "Not able to insert property.");
    $success = $importer->save_feature_property($id, 'MAIN', 'published_symbol', $symbol);
    $this->assertNotFalse($success, "Not able to update property.");
  }

  /**
   * Instantiate the importer object for tests.
   */
  private function instantiateImporter($rel_filepath, $run_args) {
    // Load our importer into scope.
    module_load_include('inc', 'tripal_map_helper', 'includes/TripalImporter/QTLImporter');

    // Create a new instance of our importer.
    $path = drupal_get_path('module', 'tripal_map_helper') . '/tests/';
    $file = ['file_local' => DRUPAL_ROOT . '/' . $path . $rel_filepath];
    $importer = new \QTLImporter();
    $importer->create($run_args, $file);

    return $importer;
  }
}
