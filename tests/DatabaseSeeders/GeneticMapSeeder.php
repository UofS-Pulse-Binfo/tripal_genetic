<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;
use Faker\Factory;

class GeneticMapSeeder extends Seeder
{
    /**
     * Seeds the database with a genetic map, linkage groups and markers.
     */
    public function up() {
      $faker = Factory::create();

      // Retrieve the cvterms I will need.
      $terms = $this->getCvterms();
      $marker_type = $faker->word();

      // First create the genetic map container.
      $featuremap = factory('chado.featuremap')->create();
      $featuremap_id = $featuremap->featuremap_id;

      // Then add the metadata.
      $meta = [
        ['term' => 'map_type', 'value' => 'genetic'],
        ['term' => 'type', 'value' => 'genetic'],
        ['term' => 'population_type', 'value' => $faker->word()],
        ['term' => 'population_size', 'value' => 144],
      ];
      foreach ($meta as $m) {
        chado_insert_record('featuremapprop', [
          'featuremap_id' => $featuremap_id,
          'type_id' => $terms[ $m['term'] ],
          'value' => $m['value'],
        ]);
      }

      // Create an organism for our map.
      $organism = factory('chado.organism')->create();
      $organism_id = $organism->organism_id;
      chado_insert_record('featuremap_organism', [
        'featuremap_id' => $featuremap_id,
        'organism_id' => $organism_id,
      ]);

      // Next create the linkage group.
      $linkage_group = factory('chado.feature')->create([
        'organism_id' => $organism_id,
        'type_id' => $terms['linkage_group'],
        'name' => 'lg1',
      ]);
      $linkage_group_id = $linkage_group->feature_id;

      // Generate 50 marker position ranging from 0-50 cM.
      $loci = factory('chado.feature', 50)->create([
        'organism_id' => $organism_id,
        'type_id' => $terms['marker_locus'],
      ]);
      foreach ($loci as $locus) {
        $locus_id = $locus->feature_id;

        // Create a genetic marker.
        $marker = factory('chado.feature')->create([
          'organism_id' => $organism_id,
          'type_id' => $terms['genetic_marker'],
        ]);
        $marker_id = $marker->feature_id;
        // Ensure the marker has a type.
        chado_insert_record('featureprop', [
          'feature_id' => $marker_id,
          'type_id' => $terms['marker_type'],
          'value' => $marker_type,
        ]);

        // Relate the marker to the locus.
        chado_insert_record('feature_relationship', [
          'subject_id' => $locus_id,
          'type_id' => $terms['instance_of'],
          'object_id' => $marker_id,
        ]);

        // Now finally position the locus on the map.
        $scale = pow(10, 2);
        $position = mt_rand(1 * $scale, 100 * $scale) / $scale;
        $pos = chado_insert_record('featurepos', [
          'featuremap_id' => $featuremap_id,
          'map_feature_id' => $linkage_group_id,
          'feature_id' => $locus_id,
          'mappos' => $position,
        ]);
        // Also add the position as a property to support ranges.
        chado_insert_record('featureposprop', [
          'featurepos_id' => $pos['featurepos_id'],
          'type_id' => $terms['start'],
          'value' => $position,
        ]);
      }

      // Return details about the genetic map.
      $this->details = [
        'featuremap_id' => $featuremap_id,
        'organism_id' => $organism_id,
        'num_LG' => 1,
        'num_loci' => 50,
        'min_pos' => 1,
        'max_pos' => 100,
      ];
    }

    public function getDetails() {
      return $this->details;
    }

    private function getCvterms() {
      $terms = [];

      $defn = [
        [
          'cv_name' => 'sequence',
          'name' => 'linkage_group'
        ],
        [
          'cv_name' => 'sequence',
          'name' => 'genetic_marker'
        ],
        [
          'cv_name' => 'MAIN',
          'name' => 'marker_locus'
        ],
        [
          'cv_name' => 'MAIN',
          'name' => 'marker_type'
        ],
        [
          'cv_name' => 'MAIN',
          'name' => 'start'
        ],
        [
          'cv_name' => 'MAIN',
          'name' => 'map_type'
        ],
        [
          'cv_name' => 'MAIN',
          'name' => 'population_type'
        ],
        [
          'cv_name' => 'MAIN',
          'name' => 'population_size'
        ],
        [
          'cv_name' => 'rdfs',
          'name' => 'type'
        ],
        [
          'cv_name' => 'relationship',
          'name' => 'instance_of'
        ],
      ];

      foreach ($defn as $d) {
        $term = chado_get_cvterm([
          'name' => $d['name'],
          'cv_id' => ['name' => $d['cv_name']]
        ]);
        $terms[ $d['name'] ] = $term->cvterm_id;
      }
      return $terms;
    }
}
