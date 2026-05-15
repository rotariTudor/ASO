using System.ComponentModel.DataAnnotations;

namespace TodoApp.Models;

public class TodoItem
{
    public int Id { get; set; }

    [Required(ErrorMessage = "Titlul este obligatoriu.")]
    [StringLength(200, ErrorMessage = "Titlul nu poate depăși 200 de caractere.")]
    public string Title { get; set; } = string.Empty;

    [StringLength(1000)]
    public string? Description { get; set; }

    public bool IsCompleted { get; set; } = false;

    public Priority Priority { get; set; } = Priority.Medium;

    public DateTime CreatedAt { get; set; } = DateTime.Now;

    public DateTime? CompletedAt { get; set; }
}

public enum Priority
{
    Low = 0,
    Medium = 1,
    High = 2
}
