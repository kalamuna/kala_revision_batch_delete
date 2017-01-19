<?php

/**
 * Admin File for Kala Revision Batch Delete.
 */

/**
 * User Delete Form.
 */
function kala_revision_batch_delete_form() {
  // Initialize Vars.
  $form = array();

  // Grab all the types.
  $types = node_type_get_types();
  foreach ($types as $type) {
    $content[$type->type] = $type->name;
  }

  // If there are content types.
  if (isset($content)) {
    // Alpah sort the array.
    asort($content);

    // Set up the content types check boxes.
    $form['content_types'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Content Types To Delete Revisions On'),
      '#required' => TRUE,
      '#options' => $content,
    );

    // Date text field.
    $form['date'] = array(
      '#type' => 'textfield',
      '#title' => t('Delete nodes revisions before this time since last node update.'),
      '#description' => t('Enter as a -1 day, 1-year, etc. Defaults to -90 days.'),
      '#default_value' => t('-90 days'),
      '#required' => TRUE,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete These Revisions!'),
      '#submit' => array('kala_revision_batch_delete_form_submit'),
    );
  }
  else {
    // No Content Types.
    $form['title'] = array(
      '#type' => 'item',
      '#markup' => '<h2><center>There are no Content Types on this site!</h2></center>',
    );
  }

  return $form;
}


/**
 * Submit handler for User Delete Form.
 */
function kala_revision_batch_delete_form_submit($form, &$form_state) {
  // Initalize vars.
  $operations = array();
  $values = $form_state['values'];

  // Grab the content types.
  if (isset($values['content_types'])) {
    foreach ($values['content_types'] as $type) {
      if ($type) {
        $types[] = $type;
      }
    }
  }

  // Grab the string date.
  if (isset($values['date'])) {
    $time = $values['date'];
  }

  // If we have above, lets do this!
  if (isset($types) && isset($time)) {

    // Splitting this into 2 queries chunks stops the time outs.
    $sql = "SELECT n.nid FROM {node} n INNER JOIN {node_revision} r ON r.nid = n.nid WHERE n.type IN (:types) GROUP BY r.nid";
    $query = db_query($sql, array(':types' => $types));
    $nids = $query->fetchAllKeyed(0,0);

    // Throw our chunks into the batch.
    $chunks = array_chunk($nids, 50);
    foreach ($chunks as $chunk) {
      $operations[] = array("kala_revision_batch_delete_batch_process", array($chunk, $time));
    }

    // Engage.
    $batch = array(
      'title' => t('Deleting Revisions...'),
      'init_message' => t('Grabbing Revisions to Delete.'),
      'operations' => $operations,
      'finished' => 'kala_revision_batch_delete_batch_finished',
      'file' => drupal_get_path('module', 'kala_revision_batch_delete') . '/includes/kala_revision_batch_delete.batch.inc',
    );

    batch_set($batch);
  }
}
