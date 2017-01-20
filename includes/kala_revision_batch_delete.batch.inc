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
 * @param int $count
 *   The total amount of vids to delete.
 *
 * @param  object &$context
 *   The batch context object.
 */
function kala_revision_batch_delete_batch_process($chunk, $count, &$context) {
  // Update our progress information.
  $context['message'] = t(':count revisions to delete total.  Deleting revisions in chunks of 50 nids.', array(':count' => $count));

  // Go through and delete the vids.
  foreach ($chunk as $vid) {
    // Will prevent deleting current revision.
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
