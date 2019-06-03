<?php
/**
 * @file
 * Implements TripalImporter to provide support for loading of genetic map files.
 */
class MSTmapImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'MSTmap Importer';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_mstmap_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Loads genetic maps (standard MSTmap format).';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['txt', 'tsv'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'MSTmap format consists of a TSV file listing the positions of markers in a genetic map. Each linkage group is labelled with it\'s name followed by a 2-column format: marker name, marker position (centimorgans). For example, <pre>
group lg0
m4	0.000
m2	0.000
m3	0.000
m1	0.000
m8	4.009
m12	4.009
m6	4.009
m7	4.009
m9	5.009
  </pre>'; 

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];


}
