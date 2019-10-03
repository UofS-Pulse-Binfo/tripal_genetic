<?php

/**
 * @class
 * Provides support for loading of MST map files.
 */
class QTLImporter extends TripalImporter {

  /**
   * The name of this loader.
   *
   * This name will be presented to the site user.
   *
   * @var string
   */
  public static $name = 'QTL Importer';

  /**
   * The machine name for this loader.
   *
   * This name will be used to construct the URL for the loader.
   *
   * @var string
   */
  public static $machine_name = 'tripal_qtl_loader';

  /**
   * A brief description for this loader.
   *
   * This description will be presented to the site user.
   *
   * @var string
   */
  public static $description = 'Loads quantitative trait loci (QTL) data.';

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
  public static $upload_description = '<p>Since QTL analysis stoftware does not output in a common format, we have compiled the key information into a simple tab-delimited format:</p>
  <ol>
    <li><strong>QTL Published Name</strong>: The name of the QTL as it was published.</li>
    <li><strong>QTL Database Name</strong>: A unique name for use within this data repository. This should follow the format <pre>[trait abbreviation][linkage group].[qtl number].[maternal]x[paternal].[siteyear]</pre> For example, "SDDIA1.2.CDCRobinx964a-46.Pres2009" would be the second QTL for Seed Diameter on linkage group 1 in the CDC Robin x 964a-46 population.</li>
    <li><strong>Trait Full Name</strong>: The full name of the trait the QTL is contributing to (e.g. Days to Flower).</li>
    <li><strong>Trait Abbreviation</strong>: A short 2-5 letter abbreviation of the trait (e.g. DTF).</li>
    <li><strong>Environments (SiteYears)</strong>: The environments a QTL is significant in (e.g. Preston 2009).</li>
    <li><strong>Peak Marker</strong>: The genetic marker closest to the peak LOD of the QTL.</li>
    <li><strong>Peak Position</strong>: The cM position of the peak LOD for the QTL.</li>
    <li><strong>Peak LOD</strong>: The highest LOD score on the QTL graph. </li>
    <li><strong>Phenotypic r2</strong>: Phenotypic variance explained by the QTL (expressed as a fraction).</li>
    <li><strong>Additive Effect</strong>: The additive effect of each allele expressed as a fraction. The parent contributing the allele is supplied in the next column. This value is usually supplied as a negative or positive value dependant upon the contributing parent.</li>
    <li><strong>Contributor Parent</strong>: The parent which contributes to the effect of the QTL based on the value of the additive effect.</li>
    <li><strong>Confidence Interval Left (1 LOD drop)</strong>:The LEFT cM position on the QTL graph where the LOD is ONE less then the peak LOD.</li>
    <li><strong>Confidence Interval Right (1 LOD drop)</strong>: The RIGHT cM position on the QTL graph where the LOD is ONE less then the peak LOD.</li>
    <li><strong>Confidence Interval Left (2 LOD drop)</strong>: The LEFT cM position on the QTL graph where the LOD is TWO less then the peak LOD. </li>
    <li><strong>Confidence Interval Right (2 LOD drop)</strong>: The RIGHT cM position on the QTL graph where the LOD is TWO less then the peak LOD.</li>
  </ol>
  <p>NOTE: Column order is important and your file is expected to contain a header.</p>';

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
    $options = chado_query('SELECT featuremap_id, name  FROM {featuremap}')->fetchAllKeyed(0,1);
    $form['featuremap_name'] = [
      '#type' => 'select',
      '#title' => 'Map Name',
      '#required' => TRUE,
      '#options' => $options,
      '#empty_option' => ' - Select Map -',
      '#description' => 'The genetic map QTL analysis was done on. This map must already be imported.',
      '#weight' => -100
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

    $success = FALSE;
    if ($success) {
      $this->logMessage('The map was imported successfully.');
    }
    else {
      $this->logMessage('An Error was encountered importing the map file.');
    }
    return $success;
  }


}
