<?php
/**
 * Batch File for Kala Revision Batch Delete.
 */

/**
 * Processing power of the batch.
 *
 * @param array $chunk
 *   The chunk of nids we are going to work on.
 *
 * @param string $time
 *   The timestamp we are checking.
 *
 * @param  object &$context
 *   The batch context object.
 */
function kala_revision_batch_delete_batch_process($chunk, $time, &$context) {
  // Update our progress information.
  $context['message'] = t('Deleting revisions in chunks of 50 nids.');

  // Grab all the nodes.
  $nodes = node_load_multiple($chunk);

  // Process each one.
  foreach ($nodes as $node) {
    // Get time difference from last update.
    $diff = strtotime($time, $node->changed);

    // Grab our revisions.
    $sql = "SELECT r.vid FROM {node_revision} r LEFT JOIN {node} n ON n.nid = r.nid WHERE r.nid = :nid AND r.timestamp < :diff AND n.vid <> r.vid ORDER BY r.timestamp DESC;";
    $query = db_query($sql, array(':nid' => $node->nid, ':diff' => $diff));


    // Go through and delete the revisions.
    foreach ($query->fetchAllKeyed(0,0) as $vid) {
      node_revision_delete($vid);
      $context['results'][] = $vid;
    }
  }
}

/*
 * Batch finished
 */
function kala_revision_batch_delete_batch_finished($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'One revision deleted.', '@count revisions deleted.');
  }
  else {
    $message = t('Assimilation was futile!');
  }
  drupal_set_message($message);

  drupal_goto('admin/config/content/revision-batch-delete');
}
