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
 * @param int $count
 *   Total nids for the message.
 *
 * @param  object &$context
 *   The batch context object.
 */
function kala_revision_batch_delete_batch_process($chunk, $time, $count, &$context) {
  // Update our progress information.
  $context['message'] = t(':total NIDs total, Deleting NID Revisions in chunks of 100', array(':total' => $count));

  // Grab our revisions.
  $sql = "SELECT r.vid FROM {node_revision} r LEFT JOIN {node} n ON n.nid = r.nid WHERE r.nid IN (:nids) AND r.timestamp < :time AND n.vid <> r.vid ORDER BY r.timestamp DESC;";
  $query = db_query($sql, array(':nids' => $chunk, ':time' => $time));

  // Go through and delete the revisions.
  foreach ($query->fetchAllKeyed(0,0) as $vid) {
    node_revision_delete($vid);
    $context['results'][] = $vid;
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
