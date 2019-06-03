<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

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
    $set = [];

    // Comprehensive (all form elements filled out.
    $set[] = [[
      'name' => 'Single Linkage Group TEST',
      'pub_map_name' => 'Something much more impressive',
      'species_abbrev' => 'Tripalus',
      'units' => 'cM',
      'map_type' => 'linkage',
      'pop_type' => 'F2',
      'pop_size' => '125',
      'contact' => 'Developer of Tripal Genetic',
      'software_name' => 'MSTmap',
      'software_version' => '1.00-teststring',
      'analysis_description' => 'I copied it from http://alumni.cs.ucr.edu/~yonghui/mstmap/example_map.txt',
      'description' => 'More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. 

More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. 

More impressive jargon which makes this map sound spectacular. 
More impressive jargon which makes this map sound spectacular. 
More impressive jargon which makes this map sound spectacular. 
More impressive jargon which makes this map sound spectacular. 

More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular. More impressive jargon which makes this map sound spectacular.',
    ]];

    // Only required.
    $set[] = [[
      'name' => 'Lazy Map',
      'species_abbrev' => 'Tripalus',
      'software_name' => 'MSTmap',
      'software_version' => 'unknown',
      'description' => 'Meh..',
    ]];

    return $set;
  }
}