<?php
/**
 * @class
 * Purpose:
 *
 * Allowing edit?
 * Data:
 * Assumptions:
 */
class so__qtl_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'QTL List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('so__qtl');


 /**
  * @see ChadoFieldWidget::form()
  *
  **/
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $widget['msg'] = [
      '#type' => 'markup',
      '#markup' => '<p>The QTL List field retrieves data from the database for display but does not provide a way to edit it. To load QTL data use the importer provided.</p>',
    ];
  }

  /**
  * @see ChadoFieldWidget::validate()
  *
  **/
  public function validate($element, $form, &$form_state, $langcode, $delta) {
  }

   /**
  * @see ChadoFieldWidget::submit()
  *
  **/
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
  }

}
