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

  // Date text field.
  $form['date'] = array(
    '#type' => 'textfield',
    '#title' => t('Delete nodes revisions before this time.'),
    '#description' => t('Enter as a -1 day, 1-year, etc. Defaults to -90 days.'),
    '#default_value' => t('-90 days'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete These Revisions!'),
    '#submit' => array('kala_revision_batch_delete_form_submit'),
  );

  return $form;
}


/**
 * Submit handler for User Delete Form.
 */
function kala_revision_batch_delete_form_submit($form, &$form_state) {
  // Initalize vars.
  $operations = array();
  $values = $form_state['values'];

  // Grab the string date.
  if (isset($values['date'])) {
    $time = strtotime($values['date']);
  }

  // If we have the time, lets do this!
  if (isset($time)) {
    // Grab all vids before our time stamp.
    $sql = "SELECT r.vid FROM {node_revision} r WHERE r.timestamp < :time ORDER BY r.timestamp DESC;";
    $query = db_query($sql, array(':time' => $time));
    $vids = $query->fetchAllKeyed(0,0);
    $count = count($vids);

    // Throw our chunks into the batch.
    $chunks = array_chunk($vids, 50);
    foreach ($chunks as $chunk) {
      $operations[] = array("kala_revision_batch_delete_batch_process", array($chunk, $count));
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
