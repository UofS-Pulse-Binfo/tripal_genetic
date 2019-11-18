<?php
/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class so__qtl extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'QTL List';

  // The default description for this field.
  public static $default_description = 'List QTL on associated Tripal Content Pages.';

  // The default widget for this field.
  public static $default_widget = 'so__qtl_widget';

  // The default formatter for this field.
  public static $default_formatter = 'so__qtl_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  // set this to 'tripal_chado'
  public static $module = 'tripal_qtl';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
     // It is expected that all fields set a 'value' in the load() function.
     // In many cases, the value may be an associative array of key/value pairs.
     // In order for Tripal to provide context for all data, the keys should
     // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
     // function that are supported by the query() function should be
     // listed here.
     'searchable_keys' => array(),
  );

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = array(
     'TripalTabDownloader',
     'TripalCSVDownloader',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'SO',
    // The name of the term.
    'term_name' => 'QTL',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000771',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  );

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

   /**
    * @see ChadoField::load()
    *
    **/

  public function load($entity) {

    // @debug dpm($entity, 'entity');
    $field_name = $this->instance['field_name'];

    // Retrieve the QTL associated with a given trait.
    if ($entity->chado_table == 'cvterm') {
      $feature_ids = chado_query('SELECT feature_id FROM {feature_cvterm} WHERE cvterm_id=:id',
        [':id' => $entity->chado_record_id])->fetchCol();
      // @debug dpm($feature_ids, 'feature_ids');
    }
    elseif ($entity->chado_table == 'featuremap') {
      $feature_ids = chado_query("SELECT qtl.feature_id FROM {featurepos} pos
        LEFT JOIN {feature} qtl ON pos.feature_id=qtl.feature_id
        WHERE qtl.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name='QTL')
        AND pos.featuremap_id=:id",
        [':id' => $entity->chado_record_id])->fetchCol();
      // @debug dpm($feature_ids, 'feature_ids');
    }

    if (!empty($feature_ids)) {

      // Retrieve the type_ids we'll need for our query.
      $type_id_obj = [
        'start' => chado_get_cvterm(['id' => 'MAIN:start']),
        'end' => chado_get_cvterm(['id' => 'MAIN:end']),
        'lod' => chado_get_cvterm(['id' => 'MAIN:lod']),
        'addt_effect' => chado_get_cvterm(['id' => 'MAIN:additive_effect']),
        'parent' => chado_get_cvterm(['id' => 'MAIN:direction']),
      ];
      $type_id = [];
      foreach ($type_id_obj as $key => $obj) {
        $type_id[$key] = 0;
        if (is_object($obj)) {
          $type_id[$key] = $obj->cvterm_id;
        }
      }

      // Now get the generic information for each QTL.
      $entity->{$field_name}['und'] = [];
      $query = "
        SELECT
          qtl.name, qtl.uniquename, qtl.feature_id as qtl_id,
          pos.mappos as peak_position, lg.name as linkage_group,
          pstart.value as cistart, pend.value as ciend,
          map.name as map_name, map.featuremap_id as map_id,
          trait.name as trait_name, trait.cvterm_id as trait_id,
          lod.value as lod, addt_effect.value as addt_effect, parent.value as parent
        FROM {feature} qtl
        LEFT JOIN {featurepos} pos ON pos.feature_id=qtl.feature_id
        LEFT JOIN {feature} lg ON lg.feature_id=pos.map_feature_id
        LEFT JOIN {featureposprop} pstart ON pstart.featurepos_id=pos.featurepos_id
          AND pstart.type_id=:start_type
        LEFT JOIN {featureposprop} pend ON pend.featurepos_id=pos.featurepos_id
          AND pend.type_id=:end_type
        LEFT JOIN {featuremap} map ON map.featuremap_id=pos.featuremap_id
        LEFT JOIN {feature_cvterm} fc ON fc.feature_id=qtl.feature_id
        LEFT JOIN {cvterm} trait ON trait.cvterm_id=fc.cvterm_id
        LEFT JOIN {featureprop} lod ON lod.feature_id=qtl.feature_id
          AND lod.type_id=:lod
        LEFT JOIN {featureprop} addt_effect ON addt_effect.feature_id=qtl.feature_id
          AND addt_effect.type_id=:addt_effect
        LEFT JOIN {featureprop} parent ON parent.feature_id=qtl.feature_id
          AND parent.type_id=:parent
        WHERE qtl.feature_id IN (:ids)
        ORDER BY map.name ASC, trait.name ASC, lg.name ASC, pos.mappos ASC";
      $QTLs = chado_query($query,
        [':ids' => $feature_ids,
         ':start_type' => $type_id['start'],
         ':end_type' => $type_id['end'],
         ':lod' => $type_id['lod'],
         ':addt_effect' => $type_id['addt_effect'],
         ':parent' => $type_id['parent']])->fetchAll();
      foreach ($QTLs as $qtl) {
        $entity->{$field_name}['und'][] = [
          'value' => [
            'data:1278' => $qtl->map_name,
            'NCIT:C85496' => $qtl->trait_name,
            'schema:name' => $qtl->name,
            'data:2091' => $qtl->uniquename,
            'SO:0000018' => $qtl->linkage_group,
            'MAIN:qtl_peak' => $qtl->peak_position,
            'MAIN:start' => $qtl->cistart,
            'MAIN:end' => $qtl->ciend,
            'MAIN:lod' => $qtl->lod,
            'MAIN:additive_effect' => $qtl->addt_effect,
            'MAIN:direction' => $qtl->parent,
          ],
          'chado-feature__feature_id' => $qtl->qtl_id,
          'map_id' => $qtl->map_id,
          'qtl_id' => $qtl->qtl_id,
          'trait_id' => $qtl->trait_id,
        ];
      }

      if (empty($entity->{$field_name}['und'])) {
        $entity->{$field_name}['und'][0]['value'] = '';
      }
    }
    // @debug dpm($entity->{$field_name}['und'], $field_name);
  }



    /**
    * @see ChadoField::query()
    *
    **/

  public function query($query, $condition) {
  }

    /**
    * @see ChadoField::queryOrder()
    *
    **/

  public function queryOrder($query, $order) {
  }


    /**
    * @see ChadoField::elementInfo()
    *
    **/

  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return array(
      $field_term => array(
        'operations' => array('eq', 'ne', 'contains', 'starts'),
        'sortable' => TRUE,
        'searchable' => TRUE,
        'label' => 'Quantitative Trait Loci (QTL)',
        'help' => ' A polymorphic locus which contains alleles that differentially affect the expression of a continuously distributed phenotypic trait.',
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => [
          'schema:name' => [
            'searchable' => TRUE,
            'label' => 'QTL Name',
            'help' => 'The published name of the quantitative trait locus.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'data:2091' => [
            'searchable' => TRUE,
            'label' => 'QTL Accession',
            'help' => 'The unique accession of the quantitative trait locus in this database.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
        ],
      ),
    );
  }

}
