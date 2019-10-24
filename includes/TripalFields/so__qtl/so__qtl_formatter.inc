<?php
/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class so__qtl_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'QTL List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('so__qtl');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

   /**
    * @see ChadoFieldFormatter::settingsForm()
    *
    **/

  public function settingsForm($view_mode, $form, &$form_state) {

  }

    /**
    * @see ChadoFieldFormatter::View()
    *
    **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $header = ['schema:name' => 'Name', 'data:2091' => 'Accession'];
    $rows = [];
    // @debug dpm($items, 'items');
    foreach ($items as $item) {
      $rows[] = [
        'schema:name' => $item['value']['schema:name'],
        'data:2091' => $item['value']['data:2091'],
      ];
    }

    $element[0] = [
      '#type' => 'theme',
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    ];

  }

    /**
    * @see ChadoFieldFormatter::settingsSummary()
    *
    **/

  public function settingsSummary($view_mode) {
    return '';
  }

}
