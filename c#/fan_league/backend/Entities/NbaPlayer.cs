namespace fan_league.Entities;

public class NbaPlayer
{
    public Guid Id { get; set; } = Guid.NewGuid();

    public int NbaId { get; set; }

    public required string FirstName { get; set; }

    public required string LastName { get; set; }

    public string? Team { get; set; }

    public string? Position { get; set; }

    public int? JerseyNumber { get; set; }

    public int? HeightCm { get; set; }

    public decimal? WeightKg { get; set; }

    public bool IsActive { get; set; } = true;

    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

    public DateTime? UpdatedAt { get; set; }

    public ICollection<NbaPlayerSeasonStat> SeasonStats { get; set; } =
        new List<NbaPlayerSeasonStat>();
}
