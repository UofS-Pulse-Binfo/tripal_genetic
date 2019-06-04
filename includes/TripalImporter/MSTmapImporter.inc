<?php

/**
 * @class
 * Provides support for loading of MST map files.
 */
class MSTmapImporter extends TripalImporter {

  /**
   * The name of this loader.
   *
   * This name will be presented to the site user.
   *
   * @var string
   */
  public static $name = 'MSTmap Importer';

  /**
   * The machine name for this loader.
   *
   * This name will be used to construct the URL for the loader.
   *
   * @var string
   */
  public static $machine_name = 'tripal_mstmap_loader';

  /**
   * A brief description for this loader.
   *
   * This description will be presented to the site user.
   *
   * @var string
   */
  public static $description = 'Loads genetic maps (standard MSTmap format).';

  /**
   * An array containing the extensions of allowed file types.
   *
   * @var array
   */
  public static $file_types = ['txt', 'tsv'];

  /**
   * Provides information to the user about the file upload.
   *
   * Typically this may include a description of the file types allowed.
   *
   * @var string
   */
  public static $upload_description = 'MSTmap format consists of a TSV file listing the positions of markers in a genetic map. Each linkage group is labelled with it\'s name followed by a 2-column format: marker name, marker position (centimorgans). For example, <pre>
group lg0
m4  0.000
m2  0.000
m3  0.000
m1  0.000
m8  4.009
m12  4.009
m6  4.009
m7  4.009
m9  5.009
  </pre>';

  /**
   * Indicates the methods that the file uploader will support.
   *
   * @var array
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];

  /**
   * Do not require an analysis.
   *
   * We create our own analysis record based on information provided and thus
   * do not need to require pre-existing analysis to be linked.
   *
   * @var bool
   */
  public static $use_analysis = FALSE;

  /**
   * Provides additional elements for the upload form including map metadata.
   */
  public function form($form, &$form_state) {

    // Map Name.
    $form['featuremap_name'] = [
      '#type' => 'textfield',
      '#title' => 'Map Name',
      '#required' => TRUE,
      '#description' => 'A unique canonical name for the linkage map as defined by the curator.',
    ];

    $form['pub_map_name'] = [
      '#type' => 'textfield',
      '#title' => 'Published Map Name',
      '#description' => 'The name of the map as it was published.',
    ];

    // Organism.
    $form['organism_organism_id'] = [
      '#type' => 'select',
      '#title' => 'Map Species',
      '#required' => TRUE,
      '#description' => "The species the map should be catagorized under based on the parental species. Cchoose interspecific if the parental species are different.",
      '#options' => chado_get_organism_select_options(FALSE),
    ];

    // Units (cM default).
    $form['featuremap_unittype_name'] = [
      '#type' => 'hidden',
      '#title' => 'Map Units',
      '#required' => TRUE,
      '#description' => 'The units the current map is in.',
      '#value' => 'cM',
    ];

    // Map Type (genetic, physical, cytogenetic, sequence).
    $form['map_type'] = [
      '#type' => 'select',
      '#title' => 'Map Type',
      '#required' => TRUE,
      '#description' => 'The type of map.',
      '#options' => [
        'linkage' => 'linkage',
        'physical' => 'physical',
        'cytogenetic' => 'cytogenetic',
        'sequence' => 'sequence',
      ],
    ];

    // Population Type (e.g. F2, BC1, RIL, ABC, etc.
    $form['pop_type'] = [
      '#type' => 'textfield',
      '#title' => 'Population Type',
      '#description' => 'The type of population used to generate the map (e.g. F2, BC1, RIL, ABC).',
    ];

    // Population Size.
    $form['pop_size'] = [
      '#type' => 'textfield',
      '#title' => 'Population Size',
      '#description' => 'The number of individuals in the population used to generate this map.',
    ];

    // Population.
    // Use a stock collection?

    // Parent 1.

    // Parent 2.

    // Contact Person.

    // Publication (if available).

    // Analysis Method.
    // Software for map construction (e.g MSTmap).
    $form['analysis_program'] = [
      '#type' => 'textfield',
      '#title' => 'Mapping Software',
      '#required' => TRUE,
      '#description' => 'The name of the mapping software used to generate the map (e.g. MSTmap, MapMaker EXP, MapMaker Macintosh, Mapdisto, CarthaGene, MergeMap, QTL IciMapping, JoinMap)',
    ];

    // Software version.
    $form['analysis_programversion'] = [
      '#type' => 'textfield',
      '#title' => 'Mapping Software Version',
      '#required' => TRUE,
      '#description' => 'The version of the above mapping software which was used.',
    ];

    // Analysis Description.
    $form['analysis_description'] = [
      '#type' => 'textarea',
      '#title' => 'Methodology',
      '#description' => 'Description of how the linkage map was created. Specifically, include software parameters, whether the markers were binned and describe any manual manipulation of the map.',
    ];

    // Map Description.
    $form['featuremap_description'] = [
      '#type' => 'textarea',
      '#title' => 'Map Description',
      '#description' => "Description of the linkage map.  Suggested topics include the published source; alternative names for the map; types of molecular markers; parents, size, and type of mapping population; linkage group number, length, marker count, and sub-genome assignment; the map's total length and marker count; average inter-marker gap distance or marker density.  Indicate if there are PCR primers that amplify more than a single locus on the map, if there is a framework map available in addition to the full genetic map, and compare and contrast with other genetic maps.  Describe whether linkage groups are homeologues and to what extent common markers and conserved marker orders are present.  Record if the map was produced directly using a bi-parental mapping population or whether it is a consensus or composite map; if it is the latter then list and describe briefly the constituent mapping populations used in its construction.  Describe how many markers were screened against the parents, what number were polymorphic and used for population genotyping, and what proportion appeared on the final map.",
    ];

    return $form;
  }

  /**
   * Validate the form input.
   */
  public function formValidate($form, &$form_state) {}

  /**
   * Import the map.
   */
  public function run() {

    // All values provided by the user in the Importer's form widgets are
    // made available to us here by the Class' arguments member variable.
    $arguments = $this->arguments['run_args'];
    print "Arguements supplied include:\n";
    foreach ($arguments as $k => $v) {
      print "\t-$k: $v\n";
    }

    // The path to the uploaded file is always made available using the
    // 'files' argument. The importer can support multiple files, therefore
    // this is an array of files, where each has a 'file_path' key specifying
    // where the file is located on the server.
    $file_path = $this->arguments['files'][0]['file_path'];

    // Save map meta-data.
    $featuremap_id = $this->loadMapMetadata($arguments);

    // @todo Save Map positions (file).
  }

  /**
   * Creater featuremap and featuremapprop records.
   */
  public function loadMapMetadata($args) {

    $feature_map = chado_insert_record('featuremap', [
      'name' => $args['featuremap_name'],
      'description' => $args['featuremap_description'],
      'unittype_id' => ['name' => $args['featuremap_unittype_name']],
    ]);

    if (!empty($feature_map)) {
      $mapid = $feature_map['featuremap_id'];

      // Create Analysis.
      $analysis = chado_insert_record('analysis', [
        'name' => 'Generate ' . $args['featuremap_name'],
        'program' => $args['analysis_program'],
        'programversion' => $args['analysis_programversion'],
        'description' => $args['analysis_description'],
      ]);

      // chado_insert_record('featuremap_analysis', [
      // 'analysis_id' => $analysis['analysis_id'],
      // 'featuremap_id' => $mapid,
      // ]);

      // chado_insert_property(
      // ['table' => 'featuremap', 'id' => $featuremap->featuremap_id],
      // );

    }
  }

}
