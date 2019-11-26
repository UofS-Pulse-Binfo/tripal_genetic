<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;
use Faker\Factory;

class QTLSeeder extends Seeder {

  /**
   * Seeds the database with a genetic map containing QTL.
   *
   * @return void
   */
  public function up() {
    $faker = Factory::create();

    // First create the genetic map.
    $seeder = GeneticMapSeeder::seed();
    $mapdetails = $seeder->getDetails();
    // @debug print "\nMAP DETAILS: ".print_r($mapdetails, TRUE);

    // Now, for each trait...
    $num_of_traits = 3;
    for ($i = 1; $i <= $num_of_traits; $i++) {

      // @debug print "Trait $i...\n";

      $scale = pow(10, 2);
      $position = mt_rand(1 * $scale, 100 * $scale) / $scale;
      $qtl_details = [
        'published_symbol' => substr(uniqid(), -4),
        'trait_name' => $faker->words(3, TRUE),
        'trait_symbol' => substr(uniqid(), -4),
        'siteyear' => $faker->word() . $faker->year(),
        'peak_marker' => 'TdChr1p' . rand(1000, 900000),
        'peak_pos' => $position,
        'peak_lod' => mt_rand(1 * $scale, 30 * $scale) / $scale,
        'r2' => mt_rand(1 * $scale, 30 * $scale) / $scale,
        'addt_effect' => mt_rand(1 * $scale, 10 * $scale) / $scale,
        'addt_parent' => 'CDC Redberry',
        'CIstart' => $position - (mt_rand(1 * $scale, 10 * $scale) / $scale),
        'CIend' => $position + (mt_rand(1 * $scale, 10 * $scale) / $scale),
        'CIlod2start' => mt_rand(1 * $scale, 15 * $scale) / $scale,
        'CIlod2end' => mt_rand(1 * $scale, 15 * $scale) / $scale,
      ];
      $qtl_details['CIlod2start'] = $qtl_details['CIlod2start'] + $qtl_details['CIstart'];
      $qtl_details['CIlod2end'] = $qtl_details['CIlod2end'] + $qtl_details['CIend'];
      // @debug print "QTL: " . print_r($qtl_details, TRUE);

      // First we need a trait.
      if (module_exists('analyzedphenotypes')) {
        $genus = chado_query('SELECT genus FROM {organism} WHERE organism_id=:id',
          [':id' => $mapdetails['organism_id']])->fetchField();
        $ap_trait = ap_insert_trait([
          'genus' => $genus,
          'name' => $qtl_details['trait_name'],
          'method_title' => $faker->words(6, TRUE),
          'method' =>  $faker->sentences(3, TRUE),
          'unit' => $faker->word,
          'type' => 'qual',
        ]);
        $trait_term = $ap_trait['trait'];
      }
      else {
        $trait_term = factory('chado.cvterm')->create([
          'name' => $qtl_details['trait_name']
        ]);
      }
      $trait_id = $trait_term->cvterm_id;
      // @debug print "TRAIT: " . print_r($trait_term,TRUE);

      // Retrieve a linkage group from the map.
      $linkage_group_id = chado_query(
        'SELECT map_feature_id FROM {featurepos} WHERE featuremap_id=:mapid',
        [':mapid' => $mapdetails['featuremap_id']])->fetchField();

      // Use the same organism as the map.
      $organism_id = $mapdetails['organism_id'];

      // Generate uniquename for QTL.
      // Tripal map assumes a non-important leading character (e.g. q)
      // then displays everything to the first period. We want to display
      // the trait symbol.
      // q[trait symbol].[linkage group]-[peak position].map[map ID].[siteyear]
      $qtl_dbname = 'q' . $qtl_details['trait_symbol'] . '.'
        . $qtl_details['linkage_group'] . '-' . $qtl_details['peak_pos']
        . '.map' . $feature_map_id . '.' . $qtl_details['siteyear'];

      // Create QTL.
      $QTL = [
        'name' => $qtl_details['published_symbol'],
        'uniquename' => $qtl_dbname,
        'organism_id' => $organism_id,
        'type_id' => ['name' => 'QTL', 'cv_id' => ['name' => 'sequence']],
      ];
      $QTL_id = $this->save_datapoint('feature', $QTL, 'feature_id');

      // Add Metadata as properties of the QTL.
      // -- Trait Abbreviation.
      $this->save_feature_property($QTL_id, 'MAIN', 'published_symbol', $qtl_details['trait_symbol']);
      // -- Environment.
      $this->save_feature_property($QTL_id, 'MAIN', 'site_name', $qtl_details['siteyear']);
      // -- Peak LOD.
      $this->save_feature_property($QTL_id, 'MAIN', 'lod', $qtl_details['peak_lod']);
      // -- r2.
      $this->save_feature_property($QTL_id, 'MAIN', 'r2', $qtl_details['r2']);
      // -- Additive Effect.
      $this->save_feature_property($QTL_id, 'MAIN', 'additive_effect', $qtl_details['addt_effect']);
      // -- Contributor Parent.
      $this->save_feature_property($QTL_id, 'MAIN', 'direction', $qtl_details['addt_parent']);

      // Associate Peak Marker.
      $this->save_feature_property($QTL_id, 'MAIN', 'marker_locus', $qtl_details['peak_marker']);

      // -- Link to Trait.
      $trait_link = [
        'feature_id' => $QTL_id,
        'cvterm_id' => $trait_id,
        'pub_id' => 1,
      ];
      $this->save_datapoint('feature_cvterm', $trait_link, 'feature_cvterm_id');

      // Add the positions.
      // -- Peak Position.
      $pos = [
        'featuremap_id' => $mapdetails['featuremap_id'],
        'map_feature_id' => $linkage_group_id,
        'feature_id' => $QTL_id,
        'mappos' => $qtl_details['peak_pos'],
      ];
      $featurepos_id = $this->save_datapoint('featurepos', $pos, 'featurepos_id');
      $this->save_feature_property($featurepos_id, 'MAIN', 'qtl_peak', $qtl_details['peak_pos'], $table = 'featurepos');

      // -- CI 1 LOD drop.
      if ($qtl_details['CIstart']) {
        $this->save_feature_property($featurepos_id, 'MAIN', 'start', $qtl_details['CIstart'], $table = 'featurepos');
        $this->save_feature_property($featurepos_id, 'MAIN', 'end', $qtl_details['CIend'], $table = 'featurepos');
      }

    }
  }

  /**
   * HELPER: Insert-Update feature properties!
   */
  public function save_feature_property($id, $cv_name, $cvterm_name, $value, $table = 'feature') {

    $record_values = [
      'table' => $table,
      'id' => $id,
    ];
    $property_values = [
      'cv_name' => $cv_name,
      'type_name' => $cvterm_name,
      'value' => $value,
    ];
    $property = chado_get_property($record_values, $property_values);

    // Insert it if it doesn't already exist.
    if (!$property) {
      return chado_insert_property($record_values, $property_values);
    }
    else {
      return chado_update_property($record_values, $property_values);
    }
  }

  /**
   * HELPER: Provides lookup functionality for this importer.
   */
  public function lookup_id($chado_table, $values, $id_column) {
    $record = chado_select_record($chado_table, ['*'], $values);
    if ($record) {
      return $record[0]->{$id_column};
    }
    else {
      return FALSE;
    }
  }

  /**
   * HELPER: Provides Insert or Update functionality for this importer.
   */
  public function save_datapoint($chado_table, $values, $id_column) {

    $record = chado_select_record($chado_table, ['*'], $values);
    if (empty($record)) {
      $record = chado_insert_record($chado_table, $values);
      return $record[$id_column];
    }
    else {
      return $record[0]->{$id_column};
    }
  }
}
