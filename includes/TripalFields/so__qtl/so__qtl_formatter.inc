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
    'title' => 'Associated quantitative trait loci (QTL).',
    'empty' => 'There are no quantitative trait loci (QTL) asscoaited with the current page.',
    'show_columns' => ['map', 'trait', 'qtl_name', 'pos', 'confidence_interval', 'peak_lod', 'additive_effect'],
  );

  /**
   * @see ChadoFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

    $default_settings = $this::$default_settings;
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    // Ensure the default are set if the value is not configured.
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    $element = [];
    $element['title'] = [
      '#type' => 'textfield',
      '#title' => 'Table Header',
      '#default_value' => $settings['title'],
    ];

    $element['empty'] = [
      '#type' => 'textfield',
      '#title' => 'Empty text',
      '#default_value' => $settings['empty'],
    ];

    $options = [
      'map' => 'Genetic Map',
      'trait' => 'Trait',
      'qtl_name' => 'QTL Name',
      'pos' => 'Position',
      'confidence_interval' => 'Confidence Interval',
      'peak_lod' => 'Peak LOD',
      'additive_effect' => 'Additive Effect',
    ];
    $element['show_columns'] = [
      '#type' => 'checkboxes',
      '#title' => 'Show the following columns:',
      '#options' => $options,
      '#default_value' => $settings['show_columns'],
    ];

    return $element;
  }

  /**
   * @see ChadoFieldFormatter::View()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];
    // Ensure the default are set if the value is not configured.
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    // If Tripal Map is installed we would like our QTL to link to it.
    $tripal_map = FALSE;
    if (module_exists('tripal_map')) {
      $tripal_map = TRUE;
    }

    $header = [
      'map' => 'Genetic Map',
      'trait' => 'Trait',
      'qtl_name' => 'Name',
      'pos' => 'Peak Position',
      'confidence_interval' => 'Confidence Interval',
      'peak_lod' => 'Peak LOD',
      'additive_effect' => 'Additive Effect',
    ];
    if ($tripal_map) {
      $header['link'] = ' ';
    }

    $rows = [];
    foreach ($items as $item) {
      if (is_array($item['value'])) {
        $row = [
          'map' => $item['value']['data:1278'],
          'trait' => $item['value']['NCIT:C85496'],
          'qtl_name' => $item['value']['schema:name'],
          'pos' => $item['value']['SO:0000018'] . ':' . $item['value']['MAIN:qtl_peak'],
          'confidence_interval' => $item['value']['MAIN:start'] . '-' . $item['value']['MAIN:end'],
          'peak_lod' => $item['value']['MAIN:lod'],
          'additive_effect' => $item['value']['MAIN:additive_effect'] . '<br />(' . $item['value']['MAIN:direction'] . ')',
        ];

        // Make links if possible.
        $this->replace_cell_with_link($row, 'map', 'featuremap', $item['map_id']);
        $this->replace_cell_with_link($row, 'qtl_name', 'feature', $item['qtl_id']);
        $this->replace_cell_with_link($row, 'trait', 'cvterm', $item['trait_id']);

        // Remove headers the admin chose to hide.
        $this->remove_columns($header, $row, $settings);

        // If Tripal Map is available then link to it.
        if ($tripal_map) {
          $mapviewer_url = strtr('mapviewer/:map_id/:lg/:qtlname', [
            ':map_id' => $item['map_id'],
            ':lg' => $item['value']['SO:0000018'],
            ':qtlname' => $item['value']['data:2091'],
          ]);
          $row['link'] = l('MapViewer', $mapviewer_url,
            ['attributes' => ['target' => '_blank']]);
        }

        $rows[] = $row;
      }
    }

    $element[0] = [
      '#type' => 'theme',
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => $settings['empty'],
      '#caption' => $settings['title'],
    ];

  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   */
  public function settingsSummary($view_mode) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    // Ensure the default are set if the value is not configured.
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    $summary = t('<strong>Title:</strong> @title<br><strong>Empty:</strong> @empty',
      [
        '@title' => $settings['title'],
        '@empty' => $settings['empty'],
      ]
    );
    return $summary;
  }

  /**
   * Replace the cell with a link.
   */
  private function replace_cell_with_link(&$row, $row_key, $table, $id) {

    $entity_id = chado_get_record_entity_by_table($table, $id);
    if ($entity_id) {
      $row[$row_key] = l($row[$row_key], '/bio_data/'.$entity_id);
    }
  }

  /**
   * Remove columns which the admin has configured to be removed.
   */
  private function remove_columns(&$header, &$row, $settings) {

    foreach ($this::$default_settings['show_columns'] as $key) {
      if (!isset($settings['show_columns'][$key])) {
        unset($row[$key]);
        if (isset($header[$key])) { unset($header[$key]); }
      }
      else {
        $v = $settings['show_columns'][$key];
        if ($v === 0) {
          unset($row[$key]);
          if (isset($header[$key])) { unset($header[$key]); }
        }
      }
    }

  }
}
